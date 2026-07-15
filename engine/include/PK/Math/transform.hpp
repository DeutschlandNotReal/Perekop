#pragma once
#include <PK/Math/number.hpp>
#include <PK/Math/simd.hpp>
#include <PK/Core/type.hpp>

#define constinl [[clang::always_inline]] constexpr

namespace pk {
    struct vec4;

    struct alignas(8) vec2 { 
        f32 x, y; 
        [[nodiscard]] constinl vec2(f32 scl) noexcept: x{scl}, y{scl} {}
        [[nodiscard]] constinl vec2(f32 x, f32 y) noexcept: x{x}, y{y} {}

        [[nodiscard]] constinl vec2 operator+(vec2 b) const noexcept { return {x+b.x, y+b.y}; }
        [[nodiscard]] constinl vec2 operator-(vec2 b) const noexcept { return {x-b.x, y-b.y}; }
        [[nodiscard]] constinl vec2 operator*(vec2 b) const noexcept { return {x*b.x, y*b.y}; }
        [[nodiscard]] constinl vec2 operator/(vec2 b) const noexcept { return {x/b.x, y/b.y}; }
        [[nodiscard]] constinl vec2 operator/(f32 scl) const noexcept { return *this * (1.f/scl); }
        [[nodiscard]] constinl vec2 operator-() const noexcept { return {-x, -y}; }

        constinl vec2& operator+=(vec2 b) noexcept { return *this = *this + b; }
        constinl vec2& operator-=(vec2 b) noexcept { return *this = *this - b; }
        constinl vec2& operator*=(vec2 b) noexcept { return *this = *this * b; }
        constinl vec2& operator/=(vec2 b) noexcept { return *this = *this / b; }
        constinl vec2& operator/=(f32 b) noexcept { return *this = *this / b; }

        [[nodiscard]] constinl f32 dot(vec2 b) const noexcept { return x*b.x + y*b.y; }
    };

    struct vec3 { 
        f32 x, y, z; 
        [[nodiscard]] constinl vec3(f32 scl) noexcept: x{scl}, y{scl}, z{scl} {}
        [[nodiscard]] constinl vec3(f32 x, f32 y, f32 z) noexcept: x{x}, y{y}, z{z} {}
        [[nodiscard]] constinl vec3(vec2 xy, f32 z = 0.f) noexcept: x{xy.x}, y{xy.y}, z{z} {}
        [[nodiscard]] constinl vec3(f32 x, vec2 yz) noexcept: x{x}, y{yz.x}, z{yz.y} {}

        [[nodiscard]] constinl vec3 operator+(vec3 b) const noexcept { return {x+b.x, y+b.y, z+b.z}; }
        [[nodiscard]] constinl vec3 operator-(vec3 b) const noexcept { return {x-b.x, y-b.y, z-b.z}; }
        [[nodiscard]] constinl vec3 operator*(vec3 b) const noexcept { return {x*b.x, y*b.y, z*b.z}; }
        [[nodiscard]] constinl vec3 operator/(vec3 b) const noexcept { return {x/b.x, y/b.y, z/b.z}; }
        [[nodiscard]] constinl vec3 operator/(f32 scl) const noexcept { return *this * (1.f/scl); }
        [[nodiscard]] constinl vec3 operator-() const noexcept { return {-x, -y, -z}; }

        constinl vec3& operator+=(vec3 b) noexcept { return *this = *this + b; }
        constinl vec3& operator-=(vec3 b) noexcept { return *this = *this - b; }
        constinl vec3& operator*=(vec3 b) noexcept { return *this = *this * b; }
        constinl vec3& operator/=(vec3 b) noexcept { return *this = *this / b; }
        constinl vec3& operator/=(f32 b) noexcept { return *this = *this / b;}

        [[nodiscard]] constinl f32 dot(vec3 b) const noexcept { return x*b.x + y*b.y + z*b.z; }
        [[nodiscard]] constinl vec3 cross(vec3 b) const noexcept {
            return {
                y * b.z - b.y * z,
                z * b.x - b.z * x,
                x * b.y - b.x * y
            };
        }
    };

    struct [[nodiscard]] quat: public simdvec<quat> {
        using simdvec<quat>::simdvec;
        friend constexpr quat operator*(quat a, quat b) noexcept;
        friend constexpr vec4 operator*(vec4 v, quat q) noexcept;
        friend constexpr vec4 operator*(quat q, vec4 v) noexcept;
        
        [[nodiscard]] constinl quat conjugate() const noexcept {
            // negates x, y, z. w uneffected
            return xormask<signbit, signbit, signbit, 0>();
        }

        [[nodiscard]] constinl quat inverse() const noexcept {
            return conjugate() * dot4(*this).rcp();
        }
    };
        
    struct [[nodiscard]] vec4 : public simdvec<vec4> {
        using simdvec<vec4>::simdvec;
        constinl vec4(vec3 xyz) noexcept { 
            if consteval { 
                x = xyz.x; y = xyz.y; z = xyz.z; w = 0.f; 
            } else {
                v = _mm_setr_ps(xyz.x, xyz.y, xyz.z, 0.f);
            }
        }

        constinl vec4(vec3 xyz, f32 w_) noexcept {
            if consteval { 
                x = xyz.x; y = xyz.y; z = xyz.z; w = w_; 
            } else {
                v = _mm_setr_ps(xyz.x, xyz.y, xyz.z, w_);
            }
        }
    };

    constinl quat operator*(quat a, quat b) noexcept {
        // w = w0*w1 - x0*x1 - y0*y1 - z0*z1
        // x = w0*x1 + x0*w1 + y0*z1 - z0*y1
        // y = w0*y1 - x0*z1 + y0*w1 + z0*x1
        // z = w0*z1 + x0*y1 - y0*x1 + z0*w1

        quat wprod = a.swizzle<3, 3, 3, 3>() * b;
        quat xprod = a.swizzle<0, 0, 0, 0>().negate<0, 1, 0, 1>() * b.swizzle<3, 2, 1, 0>(); 
        quat yprod = a.swizzle<1, 1, 1, 1>().negate<0, 0, 1, 1>() * b.swizzle<2, 3, 0, 1>();
        quat zprod = a.swizzle<2, 2, 2, 2>().negate<1, 0, 0, 1>() * b.swizzle<1, 0, 3, 2>();

        return wprod + xprod + yprod + zprod;
    }

    constinl vec4 operator*(vec4 v, quat q) noexcept {
        quat a{v.x, v.y, v.z, 0.f};

        // wprod always 0
        quat xprod = a.swizzle<0, 0, 0, 0>().negate<1, 0, 1, 0>() * q.swizzle<0, 3, 2, 1>(); 
        quat yprod = a.swizzle<1, 1, 1, 1>().negate<1, 0, 0, 1>() * q.swizzle<1, 2, 3, 0>();
        quat zprod = a.swizzle<2, 2, 2, 2>().negate<1, 1, 0, 0>() * q.swizzle<2, 1, 0, 3>();

        return bit_cast<vec4>(xprod + yprod + zprod);
    }

    constinl vec4 operator*(quat q, vec4 v) noexcept {
        return bit_cast<vec4>(q * quat{v.x, v.y, v.z, 0.f});
    }

}

#undef CONSTINL