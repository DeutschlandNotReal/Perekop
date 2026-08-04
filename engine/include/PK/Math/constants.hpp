#pragma once
#include <PK/Math/number.hpp>

namespace pk {
    template <typename T = f32> inline constexpr T pi = static_cast<T>(3.14159265358979311599796346854L);
    template <typename T = f32> inline constexpr T e  = static_cast<T>(2.71828182845904523536028747135L);

    template <typename T = f32> inline constexpr T radians(T degrees) noexcept { return degrees * (pi<> / 180); }
}