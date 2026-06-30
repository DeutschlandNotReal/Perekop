#pragma once
#include <PK/Core/memory.hpp>
#include <cstddef>
#include <initializer_list>

namespace pk {
    // dynamic array std::vector substitute
    template <typename T, u32 align = 0> class Vec {
        T *data{nullptr}, *cur{nullptr}, *cap{nullptr};

        void resize(u32 newcap) {
            if (!data) { 
                data = cur = pk::alloc<T, align>(newcap); 
                cap = data + newcap; 
                return; 
            }

            u32 len = size();
            T* ldata = data;
            data = pk::alloc<T, align>(newcap);
            pk::move(data, ldata, len);
            pk::free(ldata);
            cap = data + newcap;
            cur = data + len;
        }

        public:
            Vec() = default;
            Vec(u32 len): data(pk::alloc<T, align>(len)) { cap = data + len; cur = data; }
            Vec(const Vec &b): 
                data(pk::alloc<T, align>(b.size())) {
                cap = cur = data + b.size();
                pk::copy(data, b.data, b.size());
            }
            Vec(Vec &&b): 
                data(b.data), cap(b.cap), cur(b.cur) { 
                b.data = b.cap = b.cur = nullptr; 
            }
            Vec(std::initializer_list<T> items): data(pk::alloc<T, align>(items.size())) {
                cap = cur = data + items.size();
                pk::copy(data, items.begin(), items.size());
            }
            template <u32 L> Vec(const T (&items)[L]): data(pk::alloc<T, align>(L)) { 
                cap = cur = data + L; 
                pk::copy(data, items, L); 
            }

            explicit operator bool() const { return data != nullptr; }
            bool operator!()         const { return data == nullptr; }

            T& operator[](u32 i) { return data[i]; }
            const T& operator[](u32 i) const { return data[i]; }

            T& back() { return *(cur-1); }
            const T& back() const { return *(cur-1); }
            T* begin() { return data; }
            const T* begin() const { return data; }
            T* end() { return cur; }
            const T* end() const { return cur; }

            bool is_empty() const { return cur == data; }
            bool is_full()  const { return cap == cur; }
            bool in_range(T* ptr) const { return (size_t)ptr - (size_t)data < size(); }
            
            u32 size()     const { return cur - data; }
            u32 capacity() const { return cap - data; }
            u32 bytesize() const { return size() * sizeof(T); }

            Vec& operator=(const Vec &b) {
                if (&b == this) return *this;

                if (!data || size() < b.size()) {
                    if (data) { clear(); pk::free(data); }
                    data = pk::alloc<T, align>(b.size()); 
                    cap = data + b.size(); 
                } else { clear(); };

                pk::copy(data, b.data, b.size());
                cur = data + b.size();

                return *this;
            }

            void grow() { 
                resize(capacity() * 1.5 + 8); 
            }

            void clear() { 
                if constexpr (!std::is_trivially_destructible_v<T>)
                    while (cur > data) (--cur)->~T();
                else cur = data;
            }

            void pop() {
                if constexpr (!std::is_trivially_destructible_v<T>) 
                    (--cur)->~T(); 
                else --cur;
            }

            [[nodiscard]] T& popout() { return *--cur; }
        
            void reserve(u32 new_size) {
                if (new_size > capacity()) resize(new_size);
            }

            template <typename... A> T& emplace(A&&... args) {
                if (is_full()) grow();
                new (cur) T(forward_cast<A>(args)...);
                return *(cur++);
            }

            T& push(const T& item) {
                if (is_full()) grow();
                new (cur) T(item);
                return *(cur++);
            }

            T& push(T&& item) {
                if (is_full()) grow();
                new (cur) T(rvalue_cast(item));
                return *(cur++);
            }

            void push(std::initializer_list<T> items) {
                if (size() + items.size() > capacity()) resize(size() + items.size());

                pk::copy(cur, items.begin(), items.size());
                cur += items.size();
            }

            template <typename... A> void overide(u32 i, A&&... args) {
                new (data + i) T(forward_cast<A&&>(args)...);
            }

            void rshift(u32 i, u32 n) {
                if (size() + n > capacity()) resize(size() + n);
                pk::rshift(data + i, n);
            }

            ~Vec() {
                if (data) { clear(); pk::free(data); }
                data = cur = cap = nullptr;
            }
    };

    // refers to MUTABLE chunk of memory, std::span substitute
    template <typename T> class Span {
        T* data{nullptr}, *cap{nullptr};
        public:
            const T* begin() const { return data; }
            const T* end()   const { return cap; }
            T* begin() { return data; }
            T* end()   { return cap; }

            u32 size() const { return cap - data; }

            T& operator[](u32 i) { return data[i]; }
            const T& operator[](u32 i) const { return data[i]; }

            Span(T* single): data(single), cap(single+1) {}
            Span(T* first, T* end): data(first), cap(end) {}
            Span(T* first, u32 n): data(first), cap(first + n) {}

            template <u32 L> Span(T (&items)[L]): data(items), cap(items+L) {}

            explicit operator bool() const { return data != nullptr; }
            bool operator!()         const { return data == nullptr; }

            template <typename C> Span(C& container): 
                data((T*)container.begin()), 
                cap ((T*)container.end()) 
            {}
    };
}