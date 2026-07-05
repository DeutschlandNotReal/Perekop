#pragma once
#include <immintrin.h>
#include <bit>
#include <PK/Math/number.hpp>
#include <PK/Core/array.hpp>

#define pk_ainline [[gnu::always_inline]]

namespace pk {
    template <typename base>
    struct alignas(16) f32x4 { 
        union { struct { f32 x, y, z, w; }; __m128 xyzw; };
    
        pk_ainline constexpr f32x4(f32 scl = 0.f) noexcept: x{scl}, y{scl}, z{scl}, w{scl} {};
        pk_ainline constexpr f32x4(pk::array<f32, 4> arr) noexcept { pk::copy(&x, arr.begin(), 4); }
        pk_ainline constexpr f32x4(const f32* ptr) noexcept: xyzw(_mm_loadu_ps(ptr)) {}
        pk_ainline f32x4(__m128 xyzw) noexcept: xyzw(xyzw) {};
        pk_ainline operator __m128() const noexcept { return xyzw; }

        pk_ainline base operator/(base b) const noexcept { return _mm_div_ps(xyzw, b); }
        pk_ainline base operator+(base b) const noexcept { return _mm_add_ps(xyzw, b); }
        pk_ainline base operator-(base b) const noexcept { return _mm_sub_ps(xyzw, b); }
        pk_ainline base operator&(base b) const noexcept { return _mm_and_ps(xyzw, b); }
        pk_ainline base operator^(base b) const noexcept { return _mm_xor_ps(xyzw, b); }
        pk_ainline base operator|(base b) const noexcept { return _mm_or_ps(xyzw, b); }

        pk_ainline base& operator/=(base b) noexcept { return *this = *this / b; }
        pk_ainline base& operator+=(base b) noexcept { return *this = *this + b; }
        pk_ainline base& operator-=(base b) noexcept { return *this = *this - b; }
        pk_ainline base& operator&=(base b) noexcept { return *this = *this & b; }
        pk_ainline base& operator^=(base b) noexcept { return *this = *this ^ b; }
        pk_ainline base& operator|=(base b) noexcept { return *this = *this | b; }

        #ifdef __FMA__
        // for automatic FMA
        class mulexpr {
            friend f32x4;
            __m128 a, scl;
            public:
                pk_ainline operator f32x4() const noexcept { return _mm_mul_ps(a, scl); }
                pk_ainline operator base() const noexcept { return _mm_mul_ps(a, scl); }
                pk_ainline base operator+(base b) const noexcept { return _mm_fmadd_ps(a, scl, b); }
        };

        pk_ainline mulexpr operator*(base b) const noexcept { return {xyzw, b.xyzw}; }
        pk_ainline base operator+(mulexpr b) const noexcept { return _mm_fmadd_ps(b.a, b.scl, xyzw); }
        #else
        pk_ainline base operator*(base b) const noexcept { return _mm_mul_ps(xyzw, b); }
        #endif

        pk_ainline base& operator*=(base b) noexcept { return *this = *this * b; }

        pk_ainline base operator~() const noexcept {
            static constexpr f32x4 xormask{std::bit_cast<f32>(~0)};
            return _mm_xor_ps(xyzw, xormask);
        }

        pk_ainline base dot(base b) const noexcept {
            return _mm_dp_ps(xyzw, b, 0xFF);
        }

        template <u8 i0, u8 i1, u8 i2, u8 i3> pk_ainline base shuffle(base b) const noexcept {
            return _mm_shuffle_ps(xyzw, b, _MM_SHUFFLE(i3, i2, i1, i0));
        }

        template <u8 i0, u8 i1, u8 i2, u8 i3> pk_ainline base swizzle() const noexcept {
            return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(i3, i2, i1, i0));
        }

        pk_ainline base cross(base b) const noexcept {
            // x = a.y * b.z - b.y * a.z
            // y = a.z * b.x - b.z * a.x
            // z = a.x * b.y - b.x * a.y

            // [a.y * b.z, a.z * b.x, a.x * b.y] - [a.z * b.y, a.x * b.z, a.y * b.x]

            base lhs = swizzle<1, 2, 0, 3>() * b.template swizzle<2, 0, 1, 3>();
            base rhs = swizzle<2, 0, 1, 3>() * b.template swizzle<1, 2, 0, 3>();

            return lhs - rhs;
        }

        pk_ainline base sqrt() const noexcept { return _mm_sqrt_ps(xyzw); }
        pk_ainline base isqrt() const noexcept { return _mm_rsqrt_ps(xyzw); }
        pk_ainline base unit() const noexcept { return *this * dot(xyzw).isqrt(); }
        pk_ainline base min(base b) const noexcept { return _mm_min_ps(xyzw, b); }
        pk_ainline base max(base b) const noexcept { return _mm_max_ps(xyzw, b); }
        pk_ainline base abs() const noexcept {
            static constexpr f32x4 negmask{std::bit_cast<f32>(0x7FFFFFFF)};
            return *this & negmask;
        }
    };
}