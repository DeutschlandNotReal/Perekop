#pragma once
#include <PK/Math/number.hpp>
#include <PK/Core/memory.hpp>

namespace pk {
    template <typename T, u32 n> 
    class array {
        T data[n];
        public:
            constinl array() noexcept = default;
            constinl array(const T (&src)[n]) noexcept {
                pk::copy(data, src, n);
            }

            template <typename... arg>
            constinl array(arg... args) noexcept requires(sizeof...(args) == n && (std::is_convertible_v<arg, T> && ...))  : 
                data{static_cast<T>(args)...} 
            {}

            template <u32... ns>
            constexpr array(const array<T, ns>&... subarrays) requires((... + ns) <= n) {
                u32 i = 0;
                ([this, &i](const auto& sub){ 
                    pk::copy(data + i, sub.begin(), sub.size()); i+= sub.size(); 
                }(subarrays), ...);
            }

            constinl const T* begin() const noexcept { return data; }
            constinl const T* end()   const noexcept { return data + n; }
            constinl T* begin() noexcept { return data; }
            constinl T* end()   noexcept { return data + n; }
            constinl u32 size() const noexcept { return n; }

            constinl const T& operator[](u32 i) const noexcept { return data[i]; }
            constexpr T& operator[](u32 i) noexcept { return data[i]; }

            constinl operator const T*() const noexcept { return data; }
            constinl operator T*() noexcept { return data; }
    };
}