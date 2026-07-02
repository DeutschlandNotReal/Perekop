#pragma once
#include <PK/Core/memory.hpp>
#include <PK/Core/array.hpp>
#include <cstddef>
#include <initializer_list>

namespace pk {
    template <typename T, u32 align = 0> class vector {
        T *data{nullptr}, *cur{nullptr}, *cap{nullptr};

        void resize(u32 newcap) {
            if (data) {
                cur += pk::realloc<T, align>((T*)this, newcap);
                cap = data + newcap;
            } else {
                data = cur = pk::alloc<T, align>(newcap); 
            }
            cap = data + newcap;
        }

        T* next() {
            if (cap == cur) 
                resize((capacity() * 3 >> 1) + 8); // old * 1.5 + 8
            return cur++;
        }

        public:
            vector() = default;
            vector(u32 len): data(pk::alloc<T, align>(len)) { cap = data + len; cur = data; }
            vector(const vector &b): 
                data(pk::alloc<T, align>(b.size())) {
                cap = cur = data + b.size();
                pk::copy(data, b.data, b.size());
            }
            vector(vector &&b): 
                data(b.data), cap(b.cap), cur(b.cur) { 
                b.data = b.cap = b.cur = nullptr; 
            }
            vector(std::initializer_list<T> items): data(pk::alloc<T, align>(items.size())) {
                cap = cur = data + items.size();
                pk::copy(data, items.begin(), items.size());
            }
            template <u32 L> vector(const pk::array<T, L> &items): data(pk::alloc<T, align>(L)) { 
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
            bool in_range(T* ptr) const { return (size_t)ptr - (size_t)data < size() * sizeof(T); }
            
            u32 size()     const { return cur - data; }
            u32 capacity() const { return cap - data; }
            u32 bytesize() const { return size() * sizeof(T); }

            vector& operator=(const vector &b) {
                if (&b == this) return *this;

                if (data) {
                    clear();
                    if (capacity() < b.size()) {
                        pk::free(data);
                        data = pk::alloc<T, align>(b.size());
                        cap = data + b.size();
                    };
                } else {
                    data = pk::alloc<T, align>(b.size());
                    cap = data + b.size();
                };

                cur = data + b.size();
                pk::copy(data, b.data, b.size());

                return *this;
            }

            vector& operator=(vector &&b) {
                if (&b == this) return *this;

                if (data) { clear(); pk::free(data); }
                cur = b.cur; cap = b.cap; data = b.data;
                b.cur = b.cap = b.data = nullptr;

                return *this;
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

            void pop(T* dst) {
                pk::move(dst, *--cur);
            }

            void reserve(u32 new_size) {
                if (new_size > capacity()) resize(new_size);
            }

            template <typename... A> T& emplace(A&&... args) {
                new (next()) T(std::forward<A>(args)...);
                return back();
            }

            T& push(const T& item) {
                new (next()) T(item);
                return back();
            }

            T& push(T&& item) {
                new (next()) T(std::move(item));
                return back();
            }

            T& push(std::initializer_list<T> items) {
                if (size() + items.size() > capacity()) resize(size() + items.size());

                pk::copy(cur, items.begin(), items.size());
                T* first = cur;
                cur += items.size();
                return *first;
            }

            void shift(u32 i, u32 n) {
                if (size() + n > capacity()) resize(size() + n);
                pk::rshift(data + i, n);
            }

            ~vector() {
                if (data) { clear(); pk::free(data); }
                data = cur = cap = nullptr;
            }
    };

    // refers to MUTABLE chunk of memory, std::span substitute
    template <typename T> class Span {
        T* data{nullptr}, *cap{nullptr};
        public:
            constexpr const T* begin() const { return data; }
            constexpr const T* end()   const { return cap; }
            constexpr T* begin() { return data; }
            constexpr T* end()   { return cap; }

            constexpr u32 size() const { return cap - data; }

            constexpr T& operator[](u32 i) { return data[i]; }
            constexpr const T& operator[](u32 i) const { return data[i]; }

            constexpr Span(T* single): data(single), cap(single+1) {}
            constexpr Span(T* first, T* end): data(first), cap(end) {}
            constexpr Span(T* first, u32 n): data(first), cap(first + n) {}

            template <u32 L> constexpr Span(T (&items)[L]): data(items), cap(items+L) {}

            constexpr explicit operator bool() const { return data != nullptr; }
            constexpr bool operator!()         const { return data == nullptr; }

            template <typename C> constexpr Span(C& container): 
                data((T*)container.begin()), 
                cap ((T*)container.end()) 
            {}
    };
}