#pragma once

#include <PK/Core/span.hpp>
#include <PK/Math/extra.hpp>

#include <cstddef>
#include <initializer_list>
#include <utility>

namespace pk {
    template <typename T> class vector {
        T* data{nullptr};
        uint cur{0}, cap{0};
    
        constexpr void resize(uint newcap) {
            T* newdata = pk::alloc<T>(newcap);

            [[likely]] if (data) {
                pk::move(newdata, data, cur);
                pk::free(data);
            }

            cap = newcap;
            data = newdata;
        }

        constexpr uint nextsize() const noexcept { return (cap * 3 >> 1) + 1; }

        constexpr void grow() {
            [[unlikely]] if (full()) resize(nextsize());
        }

        public:
            constexpr vector() noexcept = default;

            constexpr vector(uint len): 
                data(pk::alloc<T>(len)), cap(len), cur(0) 
            {}

            constexpr vector(const vector &b) {
                data = pk::alloc<T>(b.size());
                cap = cur = b.size();
                pk::copy(data, b.data, b.size());
            }

            constexpr vector(vector &&b) noexcept:
                data(exchange(b.data, nullptr)),
                cap(exchange(b.cap, 0)),
                cur(exchange(b.cur, 0))
            {}

            constexpr vector(const span<T> items) { push(items); } 
            constexpr vector(std::initializer_list<T> items) { push(items); }

            constexpr explicit operator bool() const noexcept { return data != nullptr; }
            constexpr bool operator!()         const noexcept { return data == nullptr; }

            constexpr auto& operator[](this auto&& v, uint i) noexcept { return v.data[i]; }
  
            constexpr auto* begin(this auto&& v) noexcept { return v.data; }
            constexpr auto* end(this auto&& v) noexcept { return v.data + v.cur; }
            constexpr auto& back(this auto&& v) noexcept { return *(v.end()-1); }
            constexpr auto& front(this auto&& v) noexcept { return *v.data; }

            constexpr bool empty() const noexcept { return cur == 0; }
            constexpr bool full()  const noexcept { return cap == cur; }
            
            constexpr uint size()     const noexcept { return cur; }
            constexpr uint capacity() const noexcept { return cap; }
            constexpr uint bytesize() const noexcept { return size() * sizeof(T); }

            constexpr bool owns(T* ptr) const noexcept { return (size_t)ptr - (size_t)data < bytesize(); }

            constexpr vector& operator=(const vector &b) {
                if (&b == this) return *this;

                if (data) {
                    clear();
                    if (capacity() < b.size()) {
                        pk::free(data);
                        data = pk::alloc<T>(b.size());
                        cap = b.size();
                    }
                
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
                exchange(cur, b.cur, 0);
                exchange(cap, b.cap, 0);
                exchange(data, b.data, nullptr);
                return *this;
            }

            constexpr void clear() {
                if constexpr (!std::is_trivially_destructible_v<T>) 
                    while (cur) (data + --cur)->~T();
                else 
                    cur = 0;
            }

            constexpr void pop() {
                if constexpr (!std::is_trivially_destructible_v<T>) 
                    (data + --cur)->~T(); 
                else --cur;
            }

            constexpr void pop(T* dst) {
                pk::move(dst, data + --cur);
            }

            constexpr void reserve(uint newcap) {
                if (newcap > capacity()) resize(newcap);
            }

            template <typename... arg> 
            constexpr T& emplace(arg&&... args) requires(std::is_constructible_v<T, arg...>) {
                grow();
                return *std::construct_at(
                    data + cur++, 
                    std::forward<arg>(args)...
                );
            }

            constexpr T& push(T item) {
                grow();
                return *std::construct_at(data + cur++, item);
            }

            constexpr T& push(span<T> items) {
                if (size() + items.size() > nextsize())
                    resize(size() + items.size());
                else if (size() + items.size() > capacity())
                    grow(); 

                T* first = end();
                for (uint i{0}; i < items.size(); i++) std::construct_at(first+i, items[i]);
                cur += items.size();

                return *first;
            }

            constexpr T& push(std::initializer_list<T> items) {
                return push(span<T>{(T*)items.begin(), (T*)items.end()});
            }

            constexpr void shift(uint i, uint n) {
                if (size() + n > capacity()) resize(size() + n);
                pk::rshift(data + i, data + cur, n);
                cur += n;
            }

            constexpr void trim() {
                if (size() == capacity()) return;
                T* newdata = pk::alloc<T>(size());
                pk::move(newdata, data, size());
                pk::free(data);
                data = newdata;
                cap  = cur;                
            }

            constexpr ~vector() {
                if (data) { clear(); pk::free(data); }
                cap = cur = 0; data = nullptr;
            }
    };
}