#pragma once
#include <cstring>
#include <initializer_list>
using uint = unsigned int;
namespace pkutil {
    template <typename T> inline T* alloc(int n) {
       return (T*)::operator new(sizeof(T)*n);
    }

    template <typename T> inline void realloc(T*& ptr, int n, int new_n) {
        T* prev = ptr;
        ptr = copy(alloc<T>(new_n), ptr, n);
        free(prev);
    }

    template <typename T> inline T* copy(T* dst, T* src, int n) {
        std::memcpy(dst, src, n*sizeof(T));
        return dst;
    }

    template <typename T> inline void free(T* ptr) { ::operator delete(ptr); }

    template <typename T> class Array {
        T *data, *cur, *cap;

        T* next() {
            if (cur == cap) resize(size() * 2 + 1);
            return cur++;
        }

        void resize(uint new_capacity) {
            uint L = size();
            realloc(data, capacity(), new_capacity);
            cap = data + new_capacity;
            cur = data + L;
        }
        
        public:
            Array(uint len = 1): 
                data(alloc<T>(len)), cur(data), cap(data+len)
            {}

            Array(std::initializer_list<T> init): 
                data(alloc<T>(init.size())) 
            { copy(data, init.begin(), init.size()); }

            ~Array() { 
                free(data); 
            }

            T& operator[](uint i) const noexcept { 
                return data[i]; 
            }

            T* begin() const noexcept { 
                return data; 
            }

            T* end() const noexcept { 
                return cur;
            }

            T& back() const noexcept {
                 return *(cur-1); 
            }

            uint size() const noexcept { 
                return cur - data;
            }

            uint capacity() const noexcept { 
                return cap - data;
            }

            void clear() noexcept { 
                cur = data;
            }

            void pop() noexcept {
                --cur;
            }

            T& popout() noexcept { 
                return *--cur; 
            }

            T& swap_pop(uint i) noexcept { 
                return data[i] = popout(); 
            }

            void reserve(uint new_size) {
                if (new_size > capacity()) resize(new_size);
            }

            template <typename... A> void push(A&&... args) { 
                new (next()) T(args...); 
            }

            void push(const T& item) { 
                new (next()) T(item); 
            }
            void push(std::initializer_list<T> items) {
                reserve(size() + items.size());
                copy(cur, items.begin(), items.size());
                cur += items.size();
            }
            void rawpush(const T& item) {
                copy(cur++, &item, 1);
            }

            Array(const Array& b): 
                cur(b.cur), cap(b.cap), data(alloc<T>(b.size())) {
                copy(data, b.data, b.size());
            }

            Array& operator=(const Array& b) {
                free(data);
                new (this) Array(b);
                return *this;
            }

            Array(Array&& b): 
                cur(b.cur), cap(cap), data(data) {
                b.data = b.cap = b.cur = nullptr;
            }

            Array& operator=(Array&& b) {
                free<T>(data);
                cur = b.cur; cap = b.cap; data = b.data;
                b.data = b.cap = b.cur = nullptr;
                return *this;
            }
    };
}