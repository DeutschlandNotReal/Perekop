#pragma once
#include <cstring>
#include <initializer_list>
#include <type_traits>

using uint = unsigned int;
using cstring = const char*;

namespace pk {
    template <typename T> inline T* alloc(int n) {
       return (T*)::operator new(sizeof(T)*n);
    }

    inline void dealloc(void* ptr) { 
        ::operator delete(ptr); 
    }

    template <typename T, bool move = false> inline T* copy(T* dst, const T* src, int n = 1) {
        if (!dst || !src) return dst; 
        if constexpr (std::is_trivially_copyable_v<T>) 
            std::memcpy(dst, src, n*sizeof(T));
        else for (int i = 0; i < n; i++)
            if constexpr(move) dst[i] = T((T&&)src[i]); else dst[i] = T(src[i]);

        return dst;
    }

    template <typename T> class Array {
        T *data{nullptr}, *cur{nullptr}, *cap{nullptr};

        T* next() {
            if (cur == cap) 
                resize(size() * 1.5 + 8);
            return cur++;
        }

        void resize(uint newcap) {
            T* prev = data;
            data = copy<T, true>(alloc<T>(newcap), prev, size());
            cap = data + newcap;
            cur += data - prev;
            dealloc(prev);
        }
        
        public:
            Array() = default;
            Array(uint len): 
                data(alloc<T>(len)), 
                cur(data), 
                cap(data+len)
            {}

            Array(std::initializer_list<T> items): 
                data(alloc<T>(items.size()))
            { 
                cap = cur = data + items.size();
                copy<T, true>(data, items.begin(), items.size()); 
            }

            ~Array() { 
                if constexpr (!std::is_trivially_destructible_v<T>)
                    for (T* i = data; i < cur; i++)
                dealloc(data); 
            }

            T& operator[](uint i) const { return data[i]; }

            T* begin() const { return data; }
            T* end() const { return cur; }
            T& back() const { return *(cur-1); }

            uint size() const { return cur - data; }
            uint capacity() const { return cap - data; }

            void clear() { 
                if constexpr (!std::is_trivially_destructible_v<T>)
                    while (cur != data) (--cur)->~T();
                cur = data; 
            }

            void pop() { 
                if constexpr(!std::is_trivially_destructible_v<T>)
                    (--cur)->~T();
                else -- cur;
            }

            T popout() { 
                return T((T&&)*--cur); 
            }

            void reserve(uint new_size) {
                if (new_size > capacity()) resize(new_size);
            }

            template <typename... A> void emplace(A&&... args) {
                new (next()) T(args...); 
            }

            void push(const T& item) { 
                copy(next(), &item, 1);
            }

            void push(std::initializer_list<T> items) {
                reserve(size() + items.size());
                copy(cur, items.begin(), items.size());
                cur += items.size();
            }

            void push_unsafe(const T& item) {
                copy(cur++, &item);
            }

            Array(const Array& b): data(alloc<T>(b.size())) {
                copy(data, b.data, b.size());
                cap = cur = data + b.size();
            }

            Array& operator=(const Array& b) {
                if (&b == this) return *this;
                clear();
                if (capacity() < b.size()) { 
                    dealloc(data); 
                    data = alloc<T>(b.size()); 
                    cap = data + b.size(); 
                }

                copy(data, b.data, b.size());
                cur = data + b.size();

                return *this;
            }

            Array(Array&& b): cur(b.cur), cap(b.cap), data(b.data) {
                b.data = b.cap = b.cur = nullptr;
            }

            Array& operator=(Array&& b) {
                if (&b == this) return *this;
                clear(); dealloc(data);
                cur = b.cur; cap = b.cap; data = b.data;
                b.data = b.cap = b.cur = nullptr;
                return *this;
            }

            bool full() const { return cur == cap; }
            bool empty() const { return cur == data; }
        };
}