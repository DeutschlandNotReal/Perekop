#pragma once
#include <PK/Math/number.hpp>
#include <cmath>

namespace pk {
    struct alignas(8) vec2 {
        f32 x, y;
        constexpr vec2() noexcept = default;
        constexpr vec2(const f32 scl) noexcept: x{scl}, y{scl} {}
        constexpr vec2(const f32 x, const f32 y) noexcept: x{x}, y{y} {}

        constexpr vec2 operator+(const vec2 b) const noexcept { return {x+b.x, y+b.y}; }
        constexpr vec2 operator-(const vec2 b) const noexcept { return {x-b.x, y-b.y}; }
        constexpr vec2 operator-() const noexcept { return {-x, -y}; }

        constexpr vec2 operator*(const vec2 b) const noexcept { return {x*b.x, y*b.y}; }
        constexpr vec2 operator/(const vec2 b) const noexcept { return {x/b.x, y/b.y}; }
        constexpr vec2 operator*(const f32  b) const noexcept { return {x * b, y * b}; }
        constexpr vec2 operator/(const f32  b) const noexcept { return *this * (1.f / b); }
        
        constexpr vec2& operator+=(const vec2 b) noexcept { x+=b.x; y+=b.y; return *this; }
        constexpr vec2& operator-=(const vec2 b) noexcept { x-=b.x; y-=b.y; return *this; }
        constexpr vec2& operator*=(const vec2 b) noexcept { x*=b.x; y*=b.y; return *this; }
        constexpr vec2& operator/=(const vec2 b) noexcept { x/=b.x; y/=b.y; return *this; }

        constexpr vec2& operator*=(const f32 b) noexcept { x*=b; y*=b; return *this; }
        constexpr vec2& operator/=(const f32 b) noexcept { *this *= (1.f / b); return *this; }

        constexpr f32  dot(const vec2 b) const noexcept { return x*b.x + y *b.y; }
        constexpr f32  len2() const noexcept { return x*x + y*y; }
        constexpr f32  len() const noexcept { return std::sqrt(len2()); }
        constexpr vec2 unit() const noexcept { return *this / len(); }
    };
}