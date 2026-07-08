#pragma once
#include <immintrin.h>
#include <bit>
#include <PK/Math/number.hpp>
#include <PK/Core/array.hpp>

#define vecinline [[clang::always_inline]] [[clang::vectorcall]]
namespace pk {
    template <typename base>
    struct alignas(16) f32x4 { 
        union { struct { f32 x, y, z, w; }; __m128 xyzw; };
    
        vecinline constexpr f32x4(f32 scl = 0.f) noexcept: x{scl}, y{scl}, z{scl}, w{scl} {};
        vecinline constexpr f32x4(const pk::array<f32, 4> &arr) noexcept { pk::copy<f32>(this, arr, 4); }
        vecinline constexpr f32x4(const f32* ptr) noexcept: xyzw(_mm_loadu_ps(ptr)) {}
        vecinline f32x4(const __m128 &xyzw) noexcept: xyzw(xyzw) {};
        vecinline operator __m128() const noexcept { return xyzw; }

        vecinline base operator/(const base &b) const noexcept { return _mm_div_ps(xyzw, b); }
        vecinline base operator+(const base &b) const noexcept { return _mm_add_ps(xyzw, b); }
        vecinline base operator-(const base &b) const noexcept { return _mm_sub_ps(xyzw, b); }
        vecinline base operator&(const base &b) const noexcept { return _mm_and_ps(xyzw, b); }
        vecinline base operator^(const base &b) const noexcept { return _mm_xor_ps(xyzw, b); }
        vecinline base operator|(const base &b) const noexcept { return _mm_or_ps(xyzw, b); }

        vecinline base& operator/=(const base &b) noexcept { return *this = *this / b; }
        vecinline base& operator+=(const base &b) noexcept { return *this = *this + b; }
        vecinline base& operator-=(const base &b) noexcept { return *this = *this - b; }
        vecinline base& operator&=(const base &b) noexcept { return *this = *this & b; }
        vecinline base& operator^=(const base &b) noexcept { return *this = *this ^ b; }
        vecinline base& operator|=(const base &b) noexcept { return *this = *this | b; }

        #ifdef __FMA__
        // for automatic FMA
        class mulexpr {
            friend f32x4;
            __m128 a, scl;
            public:
                vecinline operator f32x4() const noexcept { return _mm_mul_ps(a, scl); }
                vecinline operator base() const noexcept { return _mm_mul_ps(a, scl); }
                vecinline base operator+(const base &b) const noexcept { return _mm_fmadd_ps(a, scl, b); }
        };

        vecinline mulexpr operator*(const base &b) const noexcept { return {xyzw, b.xyzw}; }
        vecinline base operator+(const mulexpr &b) const noexcept { return _mm_fmadd_ps(b.a, b.scl, xyzw); }
        #else
        vecinline base operator*(const base &b) const noexcept { return _mm_mul_ps(xyzw, b); }
        #endif

        vecinline base& operator*=(const base &b) noexcept { return *this = *this * b; }

        vecinline base operator~() const noexcept {
            static constexpr f32x4 xormask{std::bit_cast<f32>(~0)};
            return _mm_xor_ps(xyzw, xormask);
        }

        vecinline base dot(const base &b) const noexcept {
            return _mm_dp_ps(xyzw, b, 0xFF);
        }

        template <u8 i0, u8 i1, u8 i2, u8 i3> vecinline base shuffle(const base &b) const noexcept {
            return _mm_shuffle_ps(xyzw, b, _MM_SHUFFLE(i3, i2, i1, i0));
        }

        template <u8 i0, u8 i1, u8 i2, u8 i3> vecinline base swizzle() const noexcept {
            return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(i3, i2, i1, i0));
        }

        vecinline base cross(const base &b) const noexcept {
            // x = a.y * b.z - b.y * a.z
            // y = a.z * b.x - b.z * a.x
            // z = a.x * b.y - b.x * a.y

            // [a.y * b.z, a.z * b.x, a.x * b.y] - [a.z * b.y, a.x * b.z, a.y * b.x]

            base lhs = swizzle<1, 2, 0, 3>() * b.template swizzle<2, 0, 1, 3>();
            base rhs = swizzle<2, 0, 1, 3>() * b.template swizzle<1, 2, 0, 3>();

            return lhs - rhs;
        }

        vecinline base sqrt() const noexcept { return _mm_sqrt_ps(xyzw); }
        vecinline base isqrt() const noexcept { return _mm_rsqrt_ps(xyzw); }
        vecinline base unit() const noexcept { return *this * dot(xyzw).isqrt(); }
        vecinline base min(const base &b) const noexcept { return _mm_min_ps(xyzw, b); }
        vecinline base max(const base &b) const noexcept { return _mm_max_ps(xyzw, b); }
        vecinline base abs() const noexcept {
            static constexpr f32x4 negmask{std::bit_cast<f32>(0x7FFFFFFF)};
            return *this & negmask;
        }
    };
}