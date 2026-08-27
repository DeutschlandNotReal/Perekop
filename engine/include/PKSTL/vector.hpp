#pragma once
#include <PKSTL/memory.hpp>
#include <PKSTL/array.hpp>
#include <cstddef>
#include <initializer_list>
#include <utility>
#include <algorithm>

namespace pk {
    template <typename T, typename index = unsigned> class vector {
        T *data{nullptr}; index cap{0}, cur{0}; 

        constexpr void resize(index newcap) {
            T* newdata = pk::alloc<T>(newcap);

            [[likely]] if (data) {
                pk::move<T, false>(newdata, data, cur);
                pk::free(data);
            }

            cap = newcap;
            data = newdata;
        }

        // 1.5n + 1
        constexpr index scale_factor(index a) const noexcept { return (a * 3 >> 1) + 1; }

        constexpr void grow() {
            [[unlikely]] if (is_full()) resize(scale_factor(capacity()));
        }

        public:
            constexpr vector() noexcept = default;

            constexpr vector(index len): 
                data(pk::alloc<T>(len)), cap(len), cur(0) 
            {}

            constexpr vector(const vector &b) {
                data = pk::alloc<T>(b.size());
                cap = cur = b.size();
                pk::copy(data, b.data, b.size());
            }

            constexpr vector(vector &&b) noexcept: 
                data(b.data), cap(b.cap), cur(b.cur) {
                b.cap = b.cur = 0; b.data = nullptr;
            }

            constexpr vector(std::initializer_list<T> items) {
                data = pk::alloc<T>(items.size());
                cap = cur = items.size();
                pk::copy(data, items.begin(), items.size());
            }

            template <unsigned n> 
            constexpr vector(const pk::array<T, n> &items) {  
                data = pk::alloc<T>(n);
                cap = cur = n;
                pk::copy(data, items, n);
            }

            constexpr explicit operator bool() const noexcept { return data != nullptr; }
            constexpr bool operator!()         const noexcept { return data == nullptr; }

            constexpr T& operator[](unsigned i) noexcept { return data[i]; }
            constexpr const T& operator[](unsigned i) const noexcept { return data[i]; }

            constexpr T& back() noexcept { return *(data + cur - 1); }
            constexpr const T& back() const noexcept { return *(data + cur - 1); }
            constexpr T& front() noexcept { return *data; }
            constexpr const T& front() const noexcept { return *data; }
            constexpr T* begin() noexcept { return data; }
            constexpr const T* begin() const noexcept { return data; }
            constexpr T* end() noexcept { return data + cur; }
            constexpr const T* end() const noexcept { return data + cur; }

            constexpr bool is_empty() const noexcept { return cur == 0; }
            constexpr bool is_full()  const noexcept { return cap == cur; }
            
            constexpr index size()     const noexcept { return cur; }
            constexpr index capacity() const noexcept { return cap; }
            constexpr unsigned bytesize() const noexcept { return size() * sizeof(T); }

            constexpr bool in_range(T* ptr) const noexcept { return (size_t)ptr - (size_t)data < bytesize(); }

            constexpr vector& operator=(const vector &b) {
                if (&b == this) return *this;

                if (data) {
                    clear();
                    if (capacity() < b.size()) {
                        pk::free(data);
                        data = pk::alloc<T>(b.size());
                        cap = b.size();
                    }
                } else {
                    data = pk::alloc<T>(b.size());
                    cap = b.size();
                }

                cur = b.size();
                pk::copy(data, b.data, b.size());

                return *this;
            }

            constexpr vector& operator=(vector &&b) noexcept {
                if (&b == this) return *this;

                if (data) { clear(); pk::free(data); }
                cur = b.cur; cap = b.cap; data = b.data;
                b.cur = b.cap = 0; b.data = nullptr;

                return *this;
            }

            constexpr void clear() { 
                if constexpr (!std::is_trivially_destructible_v<T>) {
                    T* at = data + cur;
                    while (at >= data) (--at)->~T();
                }
                cur = 0;
            }

            constexpr void pop() {
                if constexpr (!std::is_trivially_destructible_v<T>) 
                    (data + --cur)->~T(); 
                else --cur;
            }

            constexpr void pop(T* dst) {
                pk::move(dst, --cur);
            }

            constexpr void reserve(unsigned new_size) {
                if (new_size > capacity()) resize(new_size);
            }

            template <typename... arg> 
            constexpr T& emplace(arg&&... args) {
                grow();
                new (data + cur++) T(std::forward<arg>(args)...);
                return back();
            }

            template <typename... item>
            constexpr T& push(item&&... items) requires((std::is_constructible_v<T, item> && ...) && sizeof...(item) > 0) {
                index oldcur{cur}, len = sizeof...(item);

                reserve(std::max(cur + len, scale_factor(cap)));
      
                (new (data + cur++) T(std::forward<item>(items)), ...);

                return *(data + oldcur);
            }

            constexpr void shift(unsigned i, unsigned n) {
                if (size() + n > capacity()) resize(size() + n);
                pk::rshift(data + i, data + cur, n);
            }

            constexpr ~vector() {
                if (data) { clear(); pk::free<>(data); }
                cap = cur = 0; data = nullptr;
            }
    };

    // refers to MUTABLE chunk of memory, std::span substitute
    template <typename T> class span {
        T* data{nullptr}, *cap{nullptr};
        public:
            constexpr const T* begin() const noexcept { return data; }
            constexpr const T* end()   const noexcept { return cap; }
            constexpr T* begin() noexcept { return data; }
            constexpr T* end()   noexcept { return cap; }

            constexpr unsigned size() const noexcept { return cap - data; }

            constexpr T& operator[](unsigned i) noexcept { return data[i]; }
            constexpr const T& operator[](unsigned i) const noexcept { return data[i]; }

            constexpr span(T* single) noexcept: data(single), cap(single+1) {}
            constexpr span(T* first, T* end) noexcept: data(first), cap(end) {}
            constexpr span(T* first, unsigned n) noexcept: data(first), cap(first + n) {}

            template <unsigned L> constexpr span(T (&items)[L]): data(items), cap(items+L) {}

            constexpr explicit operator bool() const noexcept { return data != nullptr; }
            constexpr bool operator!()         const noexcept { return data == nullptr; }

            template <typename C> 
            constexpr span(C& container): 
                data((T*)container.begin()), 
                cap ((T*)container.end()) 
            {}

            template <typename f, typename... A> 
            void map(A... args, T* result) const noexcept {
                for (unsigned i = 0; i < size(); i++) result[i] = f(data[i], std::forward(args...));
            }
    };
}