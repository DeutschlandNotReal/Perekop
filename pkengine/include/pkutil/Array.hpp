#pragma once
#include <cstring>
#include <initializer_list>

using uint = unsigned int;
namespace pk {
    template <typename T> inline T* alloc(int n) {
       return (T*)::operator new(sizeof(T)*n);
    }

    inline void dealloc(void* ptr) { 
        ::operator delete(ptr); 
    }

    template <typename T> inline T* copy(T* dst, const T* src, int n) {
        if (dst && src) std::memcpy(dst, src, n*sizeof(T));
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
            T* last = data;
            data = copy(alloc<T>(newcap), last, size());

            cap = data + newcap;
            cur += data - last;
            dealloc(last);
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
                copy(data, items.begin(), items.size()); 
            }

            ~Array() { dealloc(data); }

            T& operator[](uint i) const { return data[i]; }
            T* begin() const { return data; }
            T* end() const { return cur; }
            T& back() const { return *(cur-1); }
            uint size() const { return cur - data; }
            uint capacity() const { return cap - data; }
            void clear() { cur = data; }
            void pop() { --cur; }
            T& popout() { return *--cur; }
            T& swap_pop(uint i) { return data[i] = popout(); }

            void fit_to_size() {
                if (capacity() == size()) return;
                T* last = data;
                data = copy(alloc<T>(size()), last, size());
                cur += data - last;
                cap = cur;
            }

            void reserve(uint new_size) {
                if (new_size > capacity()) resize(new_size);
            }

            template <typename... A> void emplace(A&&... args) { 
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

            Array(const Array& b): data(alloc<T>(b.size())) {
                copy(data, b.data, b.size());
                cap = data + b.size();
                cur = data + b.capacity();
            }

            Array& operator=(const Array& b) {
                if (&b == this) return *this;
                if (capacity() < b.size()) { dealloc(data); data = alloc<T>(b.size()); cap = data + b.size(); }
                 copy(data, b.data, b.size());
                cur = data + b.size();

                return *this;
            }

            Array(Array&& b): cur(b.cur), cap(b.cap), data(b.data) {
                b.data = b.cap = b.cur = nullptr;
            }

            Array& operator=(Array&& b) {
                if (&b == this) return *this;
                dealloc(data);
                cur = b.cur; cap = b.cap; data = b.data;
                b.data = b.cap = b.cur = nullptr;
                return *this;
            }

            bool full() const { return cur == cap; }
            bool empty() const { return cur == data; }
        };
}