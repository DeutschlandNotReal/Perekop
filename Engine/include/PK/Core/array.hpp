#pragma once

#include <PK/Core/memory.hpp>

namespace pk {
    template <typename T, unsigned n = 0> 
    class array {
        T data[n];
        public:
            constexpr array() noexcept = default;
            constexpr array(const T (&src)[n]) noexcept {
                pk::copy(data, src, n);
            }

            template <typename... arg>
            constexpr array(arg... args) noexcept requires(sizeof...(args) == n && (std::is_convertible_v<arg, T> && ...))  : 
                data{static_cast<T>(args)...} 
            {}

            template <uint... ns>
            constexpr array(const array<T, ns>&... subarrays) requires((... + ns) <= n) {
                uint i = 0;
                ([this, &i](const auto& sub){ 
                    pk::copy(data + i, sub.begin(), sub.size()); i+= sub.size(); 
                }(subarrays), ...);
            }

            constexpr auto* begin(this auto&& a) noexcept { return a.data; }
            constexpr auto* end(this auto&& a) noexcept { return a.data + n; }
            constexpr auto& operator[](this auto&& a, uint i) noexcept { return a.data[i]; }

            constexpr uint size() const noexcept { return n; }
    };

    template <typename T>
    class array<T, 0> {
        T* data{nullptr};
        uint n{0};

        public:
            constexpr array() noexcept = default;

            constexpr array(uint n) noexcept: 
                n(n),
                data(pk::alloc<T>(n))
            {
                if constexpr (std::is_trivial_v<T>) {
                    std::memset(data, 0, n * sizeof(T));
                } else for (uint i{0}; i < n; i++)
                    data[i] = T();
            }

            constexpr array(std::initializer_list<T> list) noexcept {
                n = list.size();
                n = pk::alloc<T>(list.size());
                pk::move(data, list.begin(), list.size());
            }

            constexpr auto* begin(this auto&& a) noexcept { return a.data; }
            constexpr auto* end(this auto&& a) noexcept { return a.data + a.len; }
            constexpr auto& operator[](this auto&& a, uint i) noexcept { return a.data[i]; }

            constexpr uint size() const noexcept { return n; }

            constexpr void clear() noexcept {
                if (std::is_trivially_destructible_v<T>) {
                    T* last = data + n;
                    while (--last >= data) last->~T();
                } 
            }

            constexpr array(array&& val) noexcept:
                data(exchange(val.data, nullptr)),
                n(exchange(val.n, 0u))
            {}

            constexpr array& operator=(array&& val) noexcept {
                if (data) {
                    clear();
                    pk::free(data);
                }

                exchange(data, val.data, nullptr);
                exchange(n, val.n, 0u);

                
                return *this;
            }

            constexpr array(const array& val) noexcept:
                data(pk::alloc<T>(val.size())),
                n(val.size()) 
            {
                pk::copy(data, val.begin(), val.size());
            }

            constexpr array& operator=(const array& val) noexcept {
                if (data && val.size() == size()) {
                    clear();
                } else {
                    if (data) pk::free(data);
                    data = pk::alloc<T>(val.size());
                }

                pk::copy(data, val.begin(), val.size());
            }

            constexpr void dealloc() noexcept {
                if (data) {
                    clear();
                    pk::free(data);
                    n = 0; data = nullptr;
                }
            }
            
            ~array() { dealloc(); }
    };
}