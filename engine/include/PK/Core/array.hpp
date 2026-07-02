#pragma once
#include <PK/Math/number.hpp>
#include <PK/Core/memory.hpp>

namespace pk {
    template <typename T, u32 L, u32 align = alignof(T)> class alignas(align) array {
        T data[L];
        public:
            constexpr array() noexcept = default;
            constexpr array(const T (&src)[L]) noexcept {
                pk::copy(data, src, L);
            }

            constexpr const T* begin() const { return data; }
            constexpr const T* end()   const { return data + L; }
            constexpr T* begin() { return data; }
            constexpr T* end()   { return data + L; }
            constexpr u32 size() const noexcept { return L; }

            constexpr const T& operator[](u32 i) const { return data[i]; }
            constexpr T& operator[](u32 i) { return data[i]; }

            constexpr operator const T*() const { return data; }
            constexpr operator T*() { return data; }
    };
}