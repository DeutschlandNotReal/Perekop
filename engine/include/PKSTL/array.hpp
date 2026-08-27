#pragma once
#include <PKSTL/memory.hpp>

namespace pk {
    template <typename T, unsigned n> 
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

            template <unsigned... ns>
            constexpr array(const array<T, ns>&... subarrays) requires((... + ns) <= n) {
                unsigned i = 0;
                ([this, &i](const auto& sub){ 
                    pk::copy(data + i, sub.begin(), sub.size()); i+= sub.size(); 
                }(subarrays), ...);
            }

            constexpr const T* begin() const noexcept { return data; }
            constexpr const T* end()   const noexcept { return data + n; }
            constexpr T* begin() noexcept { return data; }
            constexpr T* end()   noexcept { return data + n; }
            constexpr unsigned size() const noexcept { return n; }

            constexpr const T& operator[](unsigned i) const noexcept { return data[i]; }
            constexpr T& operator[](unsigned i) noexcept { return data[i]; }

            constexpr operator const T*() const noexcept { return data; }
            constexpr operator T*() noexcept { return data; }
    };
}