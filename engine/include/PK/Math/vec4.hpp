#pragma once
#include <PK/Math/number.hpp>
#include <PK/Math/simd.hpp>
#include <cmath>

namespace pk {
    struct vec4 {
        union {
            struct { f32 x, y, z, w; };
            f32x4 v;
        };

        constexpr vec4() noexcept = default;
        constexpr vec4(const f32 scl) noexcept: x{scl}, y{scl}, z{scl}, w{scl} {}
        constexpr vec4(const f32 x, const f32 y, const f32 z = 0, const f32 w = 0) noexcept: x{x}, y{y}, z{z}, w{w} {}
        constexpr vec4(const f32x4 v) noexcept: v{v} {}

        constexpr operator f32x4() const noexcept { return v; }

        constexpr vec4 operator+(const vec4 b) const noexcept { return v+b.v; }
        constexpr vec4 operator-(const vec4 b) const noexcept { return v-b.v; }
        constexpr vec4 operator-() const noexcept { return -v; }

        constexpr vec4 operator*(const vec4 b) const noexcept { return {x*b.x, y*b.y, z*b.z}; }
        constexpr vec4 operator/(const vec4 b) const noexcept { return {x/b.x, y/b.y, z/b.z}; }
        constexpr vec4 operator*(const f32  b) const noexcept { return {x * b, y * b, z * b}; }
        constexpr vec4 operator/(const f32  b) const noexcept { return *this * (1.f / b); }
        
        constexpr vec4& operator+=(const vec4 b) noexcept { x+=b.x; y+=b.y; z+=b.z; return *this; }
        constexpr vec4& operator-=(const vec4 b) noexcept { x-=b.x; y-=b.y; z-=b.z; return *this; }
        constexpr vec4& operator*=(const vec4 b) noexcept { x*=b.x; y*=b.y; z*=b.z; return *this; }
        constexpr vec4& operator/=(const vec4 b) noexcept { x/=b.x; y/=b.y; z/=b.z; return *this; }

        constexpr vec4& operator*=(const f32 b) noexcept { x*=b; y*=b; return *this; }
        constexpr vec4& operator/=(const f32 b) noexcept { *this *= (1.f / b); return *this; }

        constexpr f32  dot(const vec4 b) const noexcept { return v.dot(b.v).x; }
        constexpr vec4 dot4(const vec4 b) const noexcept { return v.dot(b.v); }
        constexpr f32  len2() const noexcept { return v.dot(v).x; }
        constexpr f32  len() const noexcept  { return (v * v.dot(v).isqrt()).x; }
        constexpr vec4 unit() const noexcept { return v.unit(); }
    };
}