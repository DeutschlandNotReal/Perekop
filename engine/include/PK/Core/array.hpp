#pragma once
#include <PK/Math/number.hpp>
#include <PK/Core/memory.hpp>
#include <PK/Core/type.hpp>

namespace pk {
    template <typename T, u32 n> 
    class array {
        T data[n];
        public:
            constexpr array() noexcept = default;
            constexpr array(const T (&src)[n]) noexcept {
                pk::copy(data, src, n);
            }

            constexpr const T* begin() const { return data; }
            constexpr const T* end()   const { return data + n; }
            constexpr T* begin() { return data; }
            constexpr T* end()   { return data + n; }
            constexpr u32 size() const noexcept { return n; }

            constexpr const T& operator[](u32 i) const { return data[i]; }
            constexpr T& operator[](u32 i) { return data[i]; }

            constexpr operator const T*() const { return data; }
            constexpr operator T*() { return data; }
    };
}