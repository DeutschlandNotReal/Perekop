#pragma once
#include <PKSTL/memory.hpp>
#include <PKSTL/array.hpp>
#include <cstddef>
#include <initializer_list>
#include <utility>

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

        constinl T* next() {
            [[unlikely]] if (cap == cur) {
                // n * 3/2 + 1
                resize(((capacity() * 3) << 1) + 1);
            }

            return data + cur++;
        }

        public:
            constinl vector() = default;

            constexpr vector(index len): 
                data(pk::alloc<T>(len)), cap(len), cur(0) 
            {}

            constexpr vector(const  vector &b) {
                data = pk::alloc<T>(b.size());
                cap = cur = b.size();
                pk::copy(data, b.data, b.size());
            }

            constinl vector(vector &&b) noexcept: 
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

            constinl explicit operator bool() const noexcept { return data != nullptr; }
            constinl bool operator!()         const noexcept { return data == nullptr; }

            constinl T& operator[](unsigned i) noexcept { return data[i]; }
            constinl const T& operator[](unsigned i) const noexcept { return data[i]; }

            constinl T& back() noexcept { return *(data + cur - 1); }
            constinl const T& back() const noexcept { return *(data + cur - 1); }
            constinl T* begin() noexcept { return data; }
            constinl const T* begin() const noexcept { return data; }
            constinl T* end() noexcept { return data + cur; }
            constinl const T* end() const noexcept { return data + cur; }

            constinl bool is_empty() const noexcept { return cur == 0; }
            constinl bool is_full()  const noexcept { return cap == cur; }
            
            constinl index size()     const noexcept { return cur; }
            constinl index capacity() const noexcept { return cap; }
            constinl unsigned bytesize() const noexcept { return size() * sizeof(T); }

            constinl bool in_range(T* ptr) const noexcept { return (size_t)ptr - (size_t)data < bytesize(); }

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
                    T* i = data + cur;
                    while (i >= data) (i--)->~T();
                } else { cur = 0; }
            }

            constexpr void pop() {
                if constexpr (!std::is_trivially_destructible_v<T>) 
                    (data + cur--)->~T(); 
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
                new (next()) T(std::forward<arg>(args)...);
                return back();
            }

            constexpr T& push(const T& item) {
                new (next()) T(item);
                return back();
            }

            constexpr T& push(T&& item) {
                new (next()) T(std::move(item));
                return back();
            }

            constexpr T& push(std::initializer_list<T> items) {
                if (size() + items.size() > capacity()) resize(size() + items.size());

                pk::copy(data + cur, items.begin(), items.size());
    
                return data + cur += items.size() - items.size();
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
            constinl const T* begin() const noexcept { return data; }
            constinl const T* end()   const noexcept { return cap; }
            constinl T* begin() noexcept { return data; }
            constinl T* end()   noexcept { return cap; }

            constinl unsigned size() const noexcept { return cap - data; }

            constinl T& operator[](unsigned i) noexcept { return data[i]; }
            constinl const T& operator[](unsigned i) const noexcept { return data[i]; }

            constinl span(T* single): data(single), cap(single+1) {}
            constinl span(T* first, T* end): data(first), cap(end) {}
            constinl span(T* first, unsigned n): data(first), cap(first + n) {}

            template <unsigned L> constinl span(T (&items)[L]): data(items), cap(items+L) {}

            constinl explicit operator bool() const noexcept { return data != nullptr; }
            constinl bool operator!()         const noexcept { return data == nullptr; }

            template <typename C> 
            constexpr span(C& container): 
                data((T*)container.begin()), 
                cap ((T*)container.end()) 
            {}

            template <typename f, typename... A> void map(A... args, T* result) const noexcept {
                for (unsigned i = 0; i < size(); i++) 
                    result[i] = f(data[i], std::forward(args...));
            }
    };
}