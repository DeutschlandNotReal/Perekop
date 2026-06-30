#pragma once
#include <immintrin.h>
#include <PK/Core/number.hpp>

namespace pk {
    struct alignas(16) f32x4 {
        union {
            struct { f32 x, y, z, w; };
            f32 arr[4];
            __m128 m128;
        };

        constexpr f32x4() noexcept: arr{0.f, 0.f, 0.f, 0.f} {};
        constexpr f32x4(const f32 scl) noexcept: arr{scl, scl, scl, scl} {};
        constexpr f32x4(const f32 (&a)[4]) noexcept: arr{a[0], a[1], a[2], a[3]} {};

        f32x4(const f32* ptr) noexcept: m128(_mm_loadu_ps(ptr)) {}
        f32x4(const __m128 v) noexcept: m128(v) {}

        f32x4(const f32 x, const f32 y, const f32 z, const f32 w) noexcept: arr{x, y, z, w} {}

        operator __m128() const noexcept { return m128; }

        f32x4 operator*(const f32x4 b) const noexcept {
            return _mm_mul_ps(m128, b);
        }

        f32x4& operator*=(const f32x4 b) noexcept {
            m128 = _mm_mul_ps(m128, b);
            return *this;
        }

        f32x4 operator/(const f32x4 b) const noexcept {
            return _mm_div_ps(m128, b);
        }

        f32x4& operator/=(const f32x4 b) noexcept {
            m128 = _mm_div_ps(m128, b);
            return *this;
        }

        f32x4 operator+(const f32x4 b) const noexcept {
            return _mm_add_ps(m128, b);
        }

        f32x4& operator+=(const f32x4 b) noexcept {
            m128 = _mm_add_ps(m128, b);
            return *this;
        }

        f32x4 operator-(const f32x4 b) const noexcept {
            return _mm_sub_ps(m128, b);
        }

        f32x4& operator-=(const f32x4 b) noexcept {
            m128 = _mm_sub_ps(m128, b);
            return *this;
        }

        f32x4 operator&(const f32x4 b) const noexcept {
            return _mm_and_ps(m128, b);
        }

        f32x4& operator&=(const f32x4 b) noexcept {
            m128 = _mm_and_ps(m128, b);
            return *this;
        }

        f32x4 operator^(const f32x4 b) const noexcept {
            return _mm_xor_ps(m128, b);
        }

        f32x4& operator^=(const f32x4 b) noexcept {
            m128 = _mm_xor_ps(m128, b);
            return *this;
        }

        f32x4 operator|(const f32x4 b) const noexcept {
            return _mm_or_ps(m128, b);
        }

        f32x4& operator|=(const f32x4 b) noexcept {
            m128 = _mm_or_ps(m128, b);
            return *this;
        }

        f32x4 operator~() const noexcept {
            static constexpr f32x4 mask{bit_cast<f32>(~0)};
            return _mm_xor_ps(m128, mask);
        }

        f32x4 dot(const f32x4 b) const noexcept {
            return _mm_dp_ps(m128, b, 0xFF);
        }

        template <u8 i0, u8 i1, u8 i2, u8 i3> f32x4 shuffle(f32x4 b) const noexcept {
            return _mm_shuffle_ps(m128, b, _MM_SHUFFLE(i3, i2, i1, i0));
        }

        template <u8 i0, u8 i1, u8 i2, u8 i3> f32x4 swizzle() const noexcept {
            return _mm_shuffle_ps(m128, m128, _MM_SHUFFLE(i3, i2, i1, i0));
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
            return _mm_sqrt_ps(m128);
        }

        f32x4 isqrt() const noexcept {
            return _mm_rsqrt_ps(m128);
        }

        f32x4 unit() const noexcept {
            return m128 * dot(m128).isqrt();
        }

        // fused multiply add (self * scl + offset)
        f32x4 fmuladd(const f32x4 scl, const f32x4 offset) const noexcept {
            #ifdef __FMA__
                return _mm_fmadd_ps(m128, scl, offset);
            #else // for sloppy cpu like mine
                return m128 * scl + offset;
            #endif  
        }

        f32x4 min(const f32x4 b) const noexcept {
            return _mm_min_ps(m128, b);
        }

        f32x4 max(const f32x4 b) const noexcept {
            return _mm_max_ps(m128, b);
        }

        f32x4 abs() const noexcept {
            static constexpr f32x4 mask{bit_cast<f32>(0x7FFFFFFF)};
            return *this & mask;
        }

        void store(f32* ptr) const noexcept { _mm_store_ps(ptr, m128); }
    };
}