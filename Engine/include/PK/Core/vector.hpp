#pragma once

#include <PK/Core/span.hpp>
#include <PK/Math/clamp.hpp>

#include <cstddef>
#include <initializer_list>
#include <utility>

namespace pk {
    template <typename T> class vector {
        T* data{nullptr};
        uint cur{0}, cap{0};
    
        constexpr void resize(uint newcap) {
            printf("Vector resize (%i->%i)x%iB\n", cap, newcap, sizeof(T));
            T* newdata = pk::alloc<T>(newcap);

            [[likely]] if (data) {
                pk::move<T>(newdata, data, cur);
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
                data(std::exchange(b.data, nullptr)),
                cap(std::exchange(b.cap, 0)),
                cur(std::exchange(b.cur, 0))
            {}

            constexpr vector(std::initializer_list<T> items) {
                data = pk::alloc<T>(items.size());
                cap = cur = items.size();
                pk::copy(data, items.begin(), items.size());
            }

            constexpr vector(const span<T> items) {  
                data = pk::alloc<T>(items.size());
                cap = cur = items.size();
                pk::copy(data, items, items.size());
            }

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
            constexpr uint byteSize() const noexcept { return size() * sizeof(T); }

            constexpr bool inRange(T* ptr) const noexcept { return (size_t)ptr - (size_t)data < byteSize(); }

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
                cur = std::exchange(b.cur, 0);
                cap = std::exchange(b.cap, 0);
                data = std::exchange(b.data, nullptr);

                return *this;
            }

            constexpr void clear() {
                if constexpr (!std::is_trivially_destructible_v<T>) {
                    while (cur)
                        (data + --cur)->~T();
                } else {
                    cur = 0;
                }
            }

            constexpr void pop() {
                if constexpr (!std::is_trivially_destructible_v<T>) 
                    (data + --cur)->~T(); 
                else --cur;
            }

            constexpr void pop(T* dst) {
                pk::move(dst, data + --cur);
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
                uint oldcur{cur}, len = sizeof...(item);

                reserve(pk::min(cur + len, nextsize()));
      
                (new (data + cur++) T(std::forward<item>(items)), ...);

                return *(data + oldcur);
            }

            constexpr void shift(uint i, uint n) {
                if (size() + n > capacity()) resize(size() + n);
                pk::rshift(data + i, data + cur, n);
            }

            constexpr void trim() {
                T* newdata = pk::alloc<T>(size());
                pk::move(newdata, data, size());
                pk::free(data);
                data = newdata;
            }

            constexpr ~vector() {
                if (data) { clear(); pk::free<>(data); }
                cap = cur = 0; data = nullptr;
            }
    };
}