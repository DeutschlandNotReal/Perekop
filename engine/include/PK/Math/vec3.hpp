#pragma once
#include <PK/Math/number.hpp>
#include <cmath>

namespace pk {
    struct vec3 {
        f32 x, y, z;
        constexpr vec3() noexcept = default;
        constexpr vec3(const f32 scl) noexcept: x{scl}, y{scl}, z{scl} {}
        constexpr vec3(const f32 x, const f32 y, const f32 z = 0) noexcept: x{x}, y{y}, z{z} {}

        constexpr vec3 operator+(const vec3 b) const noexcept { return {x+b.x, y+b.y, z+b.z}; }
        constexpr vec3 operator-(const vec3 b) const noexcept { return {x-b.x, y-b.y, z-b.z}; }
        constexpr vec3 operator-() const noexcept { return {-x, -y, -z}; }

        constexpr vec3 operator*(const vec3 b) const noexcept { return {x*b.x, y*b.y, z*b.z}; }
        constexpr vec3 operator/(const vec3 b) const noexcept { return {x/b.x, y/b.y, z/b.z}; }
        constexpr vec3 operator*(const f32  b) const noexcept { return {x * b, y * b, z * b}; }
        constexpr vec3 operator/(const f32  b) const noexcept { return *this * (1.f / b); }
        
        constexpr vec3& operator+=(const vec3 b) noexcept { x+=b.x; y+=b.y; z+=b.z; return *this; }
        constexpr vec3& operator-=(const vec3 b) noexcept { x-=b.x; y-=b.y; z-=b.z; return *this; }
        constexpr vec3& operator*=(const vec3 b) noexcept { x*=b.x; y*=b.y; z*=b.z; return *this; }
        constexpr vec3& operator/=(const vec3 b) noexcept { x/=b.x; y/=b.y; z/=b.z; return *this; }

        constexpr vec3& operator*=(const f32 b) noexcept { x*=b; y*=b; return *this; }
        constexpr vec3& operator/=(const f32 b) noexcept { *this *= (1.f / b); return *this; }

        constexpr f32  dot(const vec3 b) const noexcept { return x*b.x + y*b.y + z*b.z; }
        constexpr vec3 cross(const vec3 b) const noexcept { return {y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x}; }
        constexpr f32  len2() const noexcept { return x*x + y*y + z*z; }
        constexpr f32  len() const noexcept { return std::sqrt(len2()); }
        constexpr vec3 unit() const noexcept { return *this / len(); }
    };
}