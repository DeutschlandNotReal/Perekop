#pragma once
#include <PKCore/memory.hpp>
#include <initializer_list>
#include <type_traits>

#if defined(PK_DEBUG_VEC) && PK_DEBUG_VEC != 0
#include <PKCore/debug.hpp>
// debug flags:
// 0b0001: vector index OOB
// 0b0010: vector growth
// 0b0100: vector reserve ignored
#endif

namespace pk {

    // dynamic array std::vector substitute
    template <typename T> class vector {
        T *data{nullptr}, *cur{nullptr}, *cap{nullptr};

        void resize(uint32_t ncap) {
            if (!data) { data = cur = PKAlloc<T>(ncap); cap = data + ncap; return; }

            #if defined(PK_DEBUG_VEC) && (PK_DEBUG_VEC & 0b0010) 
                printf("(%s) vector<%s> resize (%u -> %u)\n", PK_DEBUG, classname<T>, capacity(), ncap);
            #endif

            uint32_t len = size();
            T* ldata = data;
            data = PKAlloc<T>(ncap);
            PKMove(data, ldata, len);
            cur = data + len;
            cap = data + ncap;
            PKFree(ldata);
        }

        T* next() {
            if (cur == cap) grow();
            return cur++;
        }

        public:
            void grow() { resize(capacity() * 1.5 + 8); }

            vector() = default;
            vector(uint32_t len): data(PKAlloc<T>(len)) { cap = data + len; cur = data; }

            vector(std::initializer_list<T> items): data(PKAlloc<T>(items.size())) {
                cap = cur = data + items.size();
                PKCopy(data, items.begin(), items.size());
            }

            template <uint32_t L> vector(const T (&items)[L]): data(PKAlloc<T>(L)) { 
                cap = cur = data + L; 
                PKCopy(data, items, L); 
            }
            
            [[nodiscard]] explicit operator bool() const { return data != nullptr; }
            [[nodiscard]] bool operator!() const { return data == nullptr; }

            T& operator[](uint32_t i) const {
                #if defined(PK_DEBUG_VEC) && (PK_DEBUG_VEC & 0b0010) 
                    if (i >= size())
                        printf("\033[31m(%s) ERROR: vector<%s> OOB [%i/%u]\n\033[0m", PK_DEBUG, classname<T>, i, size());
                    else if (!data)
                        printf("\033[31m(%s) ERROR: vector<%s> OOB [%i/nil]\n\033[0m", PK_DEBUG, classname<T>, i);
                #endif
                return data[i]; 
            }

            T& back()  const { return operator[](size() - 1); }
            T* begin() const { return data; }
            T* end()   const { return cur; }

            [[nodiscard]] bool empty() const { return cur == data; }
            [[nodiscard]] bool full()  const { return cap == cur; }
            
            uint32_t size()     const { return cur - data; }
            uint32_t capacity() const { return cap - data; }
            uint32_t bytesize() const { return size() * sizeof(T); }

            vector(vector&& b): data(b.data), cap(b.cap), cur(b.cur) { b.data = b.cap = b.cur = nullptr; }
            vector& operator=(vector&& b) {
                if (&b == this) return *this;
                if (data) { clear(); PKFree(data); }
                data = b.data; cap = b.cap; cur = b.cur;
                b.data = b.cap = b.cur = nullptr;

                return *this;
            }

            vector(const vector& b): data(PKAlloc<T>(b.size())) {
                cap = cur = data + b.size();
                PKCopy(data, b.data, b.size());
            }
            
            vector& operator=(const vector& b) { *this;
                if (&b == this) return *this;

                if (data) {
                    clear();
                    if (size() < b.size()) { 
                        PKFree(data); 
                        data = PKAlloc<T>(b.size()); 
                        cap = cur = data + b.size(); 
                    }
                } else { data = PKAlloc<T>(b.size()); cap = data + b.size(); }

                PKCopy(data, b.data, b.size());
                cur = data + b.size();

                return *this;
            }

            void clear() { 
                if constexpr (!std::is_trivially_destructible_v<T>)
                    for (T* i = cur-1; i >= data; i--) i->~T();
                cur = data; 
            }

            void pop() {
                if constexpr (!std::is_trivially_destructible_v<T>) {
                    (--cur)->~T();
                } else { --cur; }
            }


            void popout(T* to) {
                new (to) T((T&&) *--cur); // move out
            }

            void reserve(uint32_t new_size) {
                if (new_size > capacity()) resize(new_size);
                #if defined(PK_DEBUG_VEC) && (PK_DEBUG_VEC & 0b0100) 
                    else printf("(%s) vector<%s> reserve ignored [%u/%u]\n", PK_DEBUG, classname<T>, new_size, size());
                #endif
            }

            template <typename... A> T& emplace(A&&... args) {
                new (next()) T(args...);
                return back(); 
            }

            // unsafe emplace
            template <typename... A> T& u_emplace(A&&... args) {
                new (cur++) T(args...);
                return back();
            }

            T& push(const T& item) { 
                PKCopy(next(), &item);
                return back();
            }

            // unsafe push
            T& u_push(const T& item) {
                PKCopy(cur++, &item);
                return back();
            }

            void push(std::initializer_list<T> items) {
                reserve(size() + items.size());
                PKCopy(cur, items.begin(), items.size());
                cur += items.size();
            }

            ~vector() {
                if (!data) return;
                clear();
                PKFree(data);
            }
    };

    // referance to array data without owning, substitute for std::span
    template <typename T> class refarray {
        T *data{nullptr}, *cap{nullptr};
        public:
            T* begin() const { return data; }
            T* end()   const { return cap; }
            T& operator[](uint32_t i) const { return data[i]; }
            uint32_t size() const { return cap - data; }

            refarray(const T&) = delete;
            refarray(T& i): data(&i), cap(&i+1) {}
            refarray(const vector<T>& a): data(a.begin()), cap(a.end()) {}

            template <uint32_t L> refarray(T (&items)[L]): data(items), cap(items+L) {}
    };
}