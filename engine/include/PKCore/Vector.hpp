#pragma once
#include <PKCore/Memory.hpp>
#include <initializer_list>
#include <type_traits>

#ifdef PK_DEBUG
#define PK_DEBUG_ARRAY PK_DEBUG
#endif

#ifdef PK_DEBUG_ARRAY
#include <PKCore/Debug.hpp>
#endif

namespace pk {
    template <typename T> class vector {
        T *data{nullptr}, *cur{nullptr}, *cap{nullptr};

        void resize(uint32_t ncap) {
            if (!data) { data = cur = PKAlloc<T>(ncap); cap = data + ncap; return; }
            #ifdef PK_DEBUG_ARRAY
            printf("(%s) vector<%s> resize (%u -> %u)\n", PK_DEBUG_ARRAY, pk::t_name<T>(), capacity(), ncap);
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
            if (cur == cap) resize(capacity() * 1.5 + 8);
            return cur++;
        }

        public:
            vector() = default;
            vector(uint32_t len): data(PKAlloc<T>(len)) { cap = data + len; cur = data; }

            // was like this before vvv
            // vector(uint32_t len): data(PKAlloc<T>(len)) { cap = cur = data + len; }
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
                #ifdef PK_DEBUG_ARRAY
                if (i >= size())
                    printf("\033[31m(%s) ERROR: vector<%s> OOB [%i/%u]\n\033[0m", PK_DEBUG_ARRAY, pk::t_name<T>(), i, size());
                else if (!data)
                    printf("\033[31m(%s) ERROR: vector<%s> OOB [%i/nil]\n\033[0m", PK_DEBUG_ARRAY, pk::t_name<T>(), i);
                
                #endif
                return data[i]; 
            }

            T& back()  const { return *(cur-1); }
            T* begin() const { return data; }
            T* end()   const { return cur; }
            [[nodiscard]] bool empty() const { return cur == data; }
            [[nodiscard]] bool full()  const { return cap == cur; }
            
            uint32_t size() const { return cur - data; }
            uint32_t capacity() const { return cap - data; }

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
                if constexpr(!std::is_trivially_destructible_v<T>)
                    (--cur)->~T();
                else --cur;
            }

            [[nodiscard]] T&& popout() { 
                return *--cur;
            }

            void reserve(uint32_t new_size) {
                if (new_size > capacity()) resize(new_size);
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
                PKCopy(next(), &item, 1);
                return back();
            }

            // unsafe push
            T& u_push(const T& item) {
                PKCopy(cur++, &item, 1);
                return back();
            }

            void push(std::initializer_list<T> items) {
                reserve(size() + items.size());
                PKCopy(cur, items.begin(), items.size());
                cur += items.size();
            }

            ~vector() {
                if (data) {
                    clear();
                    PKFree(data);
                };
            }
    };

    // For viewers who just read array data (dont care if static or dynamic, only reads)
    template <typename T> class arrayview {
        T *data{nullptr}, *cap{nullptr};
        public:
            T* begin() const { return data; }
            T* end()   const { return cap; }
            T& operator[](uint32_t i) const { return data[i]; }
            uint32_t size() const { return cap - data; }

            arrayview(const T&) = delete;
            arrayview(T& i): data(&i), cap(&i+1) {}
            arrayview(const vector<T>& a): data(a.begin()), cap(a.end()) {}

            template <uint32_t L> arrayview(T (&items)[L]): data(items), cap(items+L) {}
    };
}