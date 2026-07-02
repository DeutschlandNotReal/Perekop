#pragma once
#include <immintrin.h>
#include <PK/Math/number.hpp>

namespace pk {
    struct alignas(16) f32x4 {
        union {
            struct { f32 x, y, z, w; };
            __m128 vec;
        };

        constexpr f32x4(const f32 scl = 0.f) noexcept: x{scl}, y{scl}, z{scl}, w{scl} {};
        constexpr f32x4(const f32 (&a)[4]) noexcept: x{a[0]}, y{a[1]}, z{a[2]}, w{a[3]} {};

        f32x4(const f32* ptr) noexcept: vec(_mm_loadu_ps(ptr)) {}
        f32x4(const __m128 v) noexcept: vec(v) {}

        constexpr f32x4(const f32 x, const f32 y, const f32 z = 0, const f32 w = 0) noexcept: x{x}, y{y}, z{z}, w{w} {}

        operator __m128() const noexcept { return vec; }

        constexpr f32x4 operator=(const f32x4 b) noexcept {
            vec = b.vec;
            return *this;
        }

        constexpr f32  operator[](u8 i) const noexcept { return *(&x + i); }
        constexpr f32& operator[](u8 i) noexcept { return *(&x + i); }

        f32x4 operator*(const f32x4 b) const noexcept {
            return _mm_mul_ps(vec, b);
        }

        f32x4& operator*=(const f32x4 b) noexcept {
            vec = _mm_mul_ps(vec, b);
            return *this;
        }

        f32x4 operator/(const f32x4 b) const noexcept {
            return _mm_div_ps(vec, b);
        }

        f32x4& operator/=(const f32x4 b) noexcept {
            vec = _mm_div_ps(vec, b);
            return *this;
        }

        f32x4 operator+(const f32x4 b) const noexcept {
            return _mm_add_ps(vec, b);
        }

        f32x4& operator+=(const f32x4 b) noexcept {
            vec = _mm_add_ps(vec, b);
            return *this;
        }

        f32x4 operator-(const f32x4 b) const noexcept {
            return _mm_sub_ps(vec, b);
        }

        f32x4 operator-() const noexcept {
            static constexpr f32x4 mask{bit_cast<f32>(0x80000000)};
            return _mm_xor_ps(vec, mask);
        }

        f32x4& operator-=(const f32x4 b) noexcept {
            vec = _mm_sub_ps(vec, b);
            return *this;
        }

        f32x4 operator&(const f32x4 b) const noexcept {
            return _mm_and_ps(vec, b);
        }

        f32x4& operator&=(const f32x4 b) noexcept {
            vec = _mm_and_ps(vec, b);
            return *this;
        }

        f32x4 operator^(const f32x4 b) const noexcept {
            return _mm_xor_ps(vec, b);
        }

        f32x4& operator^=(const f32x4 b) noexcept {
            vec = _mm_xor_ps(vec, b);
            return *this;
        }

        f32x4 operator|(const f32x4 b) const noexcept {
            return _mm_or_ps(vec, b);
        }

        f32x4& operator|=(const f32x4 b) noexcept {
            vec = _mm_or_ps(vec, b);
            return *this;
        }

        f32x4 operator~() const noexcept {
            static constexpr f32x4 mask{bit_cast<f32>(~0)};
            return _mm_xor_ps(vec, mask);
        }

        f32x4 dot(const f32x4 b) const noexcept {
            return _mm_dp_ps(vec, b, 0xFF);
        }

        template <u8 i0, u8 i1, u8 i2, u8 i3> f32x4 shuffle(f32x4 b) const noexcept {
            return _mm_shuffle_ps(vec, b, _MM_SHUFFLE(i3, i2, i1, i0));
        }

        template <u8 i0, u8 i1, u8 i2, u8 i3> f32x4 swizzle() const noexcept {
            return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(i3, i2, i1, i0));
        }

        f32x4 cross(const f32x4 b) const noexcept {
            // x = a.y * b.z - b.y * a.z
            // y = a.z * b.x - b.z * a.x
            // z = a.x * b.y - b.x * a.y

            // [a.y * b.y, a.z * b.x, a.x * b.y] - [a.z * b.y, a.x * b.z, a.y * b.x]
            f32x4 lhs = swizzle<1, 2, 0, 3>() * b.swizzle<2, 0, 1, 3>();
            f32x4 rhs = swizzle<2, 0, 1, 3>() * b.swizzle<1, 2, 0, 3>();

            return lhs - rhs;
        }

        f32x4 sqrt() const noexcept {
            return _mm_sqrt_ps(vec);
        }

        f32x4 isqrt() const noexcept {
            return _mm_rsqrt_ps(vec);
        }

        f32x4 unit() const noexcept {
            return vec * dot(vec).isqrt();
        }

        // fused multiply add (self * scl + offset)
        f32x4 fmuladd(const f32x4 scl, const f32x4 offset) const noexcept {
            #ifdef __FMA__
                return _mm_fmadd_ps(vec, scl, offset);
            #else
                return vec * scl + offset;
            #endif  
        }

        f32x4 min(const f32x4 b) const noexcept {
            return _mm_min_ps(vec, b);
        }

        f32x4 max(const f32x4 b) const noexcept {
            return _mm_max_ps(vec, b);
        }

        f32x4 abs() const noexcept {
            static constexpr f32x4 mask{bit_cast<f32>(0x7FFFFFFF)};
            return *this & mask;
        }
    };
}