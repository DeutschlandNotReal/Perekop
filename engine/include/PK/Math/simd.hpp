#pragma once
#include <immintrin.h>
#include <bit>
#include <PK/Math/number.hpp>
#include <cmath>
using std::bit_cast;

#ifndef constinl
#define constinl [[clang::always_inline]] constexpr
#endif

#define F32BIT(sym) [](f32 a, f32 b) { return bit_cast<f32>(bit_cast<u32>(a) sym bit_cast<u32>(b)); }
#define F32SCL(sym) [](f32 a, f32 b) { return a sym b; }

namespace pk {
    static inline constexpr u32 signbit = 0x80000000;
    
    struct alignas(16) f32x4 {
        union { __m128 v; struct { f32 x, y, z, w; }; };

        constinl f32x4(__m128 v) noexcept: v(v) {}

        constinl operator __m128() const noexcept { return v; }

        [[nodiscard]] constinl f32x4() = default;
        [[nodiscard]] constinl f32x4(f32 scl) noexcept {
            if consteval { x = y = z = w = scl; } else { v = _mm_set1_ps(scl); }
        }

        [[nodiscard]] constinl f32x4(f32 x, f32 y, f32 z, f32 w) noexcept: x{x}, y{y}, z{z}, w{w} {}

        constinl void store(f32* ptr) noexcept {
            if consteval {
                *ptr++ = x; *ptr++ = y; *ptr++ = z; *ptr = w;
            } else {
                _mm_storeu_ps(ptr, v);
            }
        }

        template <typename X, typename Y, typename Z, typename W>
        [[nodiscard]] constinl static f32x4 from_bit(X x, Y y, Z z, W w) noexcept {
            return {bit_cast<f32>(x), bit_cast<f32>(y), bit_cast<f32>(z), bit_cast<f32>(w)};
        }

        template <auto rt, auto ct, typename... args>
        [[nodiscard]] constinl f32x4 map(args... arg) const noexcept  {
            if consteval { 
                return {rt(x, arg.x...), rt(y, arg.y...), rt(z, arg.z...), rt(w, arg.w...)};
            } else { 
                return ct(*this, arg...); 
            }
        }

        [[nodiscard]] constinl f32x4 rcp() const noexcept {
            if consteval {
                return {1.f/x, 1.f/y, 1.f/z, 1.f/w};
            } else {
                f32x4 y = _mm_rcp_ps(v);

                return y *= (2.f - v * y);
            }
        }

        [[nodiscard]] constinl f32x4 rsqrt() const noexcept {
            if consteval {
                f32 y = 1.f / std::sqrtf(x);
                
                return {y, y, y, y};
            } else {
                f32x4 y = _mm_rsqrt_ps(v);

                return y *= (1.5f - .5f * v * y * y);
            }
        }

        [[nodiscard]] constinl f32x4 sqrt() const noexcept {
            return rsqrt() * *this;
        }

        template <u8 X, u8 Y, u8 Z, u8 W> 
        [[nodiscard]] constinl f32x4 shuffle(const f32x4 &b) const noexcept {
            if consteval { 
                return {(*this)[X], (*this)[Y], b[Z], b[W]}; 
            } else {
                return _mm_shuffle_ps(*this, b, _MM_SHUFFLE(W, Z, Y, X));
            }
        }

        template <u8 X, u8 Y, u8 Z, u8 W> 
        [[nodiscard]] constinl f32x4 swizzle() const noexcept {
            return shuffle<X, Y, Z, W>(*this);
        }

        template <u32 X, u32 Y = X, u32 Z = X, u32 W = X> [[nodiscard]] constinl f32x4 andmask() const noexcept { 
            return map<F32BIT(&), _mm_and_ps>(from_bit(X, Y, Z, W));
        }

        template <u32 X, u32 Y = X, u32 Z = X, u32 W = X> [[nodiscard]] constinl f32x4 xormask() const noexcept { 
            return map<F32BIT(^), _mm_xor_ps>(from_bit(X, Y, Z, W));
        }
 
        template <u32 X, u32 Y = X, u32 Z = X, u32 W = X> [[nodiscard]] constinl f32x4 ormask() const noexcept {
            return map<F32BIT(|), _mm_or_ps>(from_bit(X, Y, Z, W));
        }

        template <bool X, bool Y, bool Z, bool W> [[nodiscard]] constinl f32x4 negate() const noexcept {
            return xormask<X*signbit, Y*signbit, Z*signbit, W*signbit>();
        }

        template <u8 mode = 0xFF> 
        [[nodiscard]] constinl f32x4 dot(f32x4 b) const noexcept {
            if consteval {
                f32 dp = 0;
                if constexpr (mode & 0x80) dp += x * b.x;
                if constexpr (mode & 0x40) dp += y * b.y;
                if constexpr (mode & 0x20) dp += z * b.z;
                if constexpr (mode & 0x10) dp += w * b.w;

                return f32x4{
                    (mode & 8) ? dp : 0.f,
                    (mode & 4) ? dp : 0.f,
                    (mode & 2) ? dp : 0.f,
                    (mode & 1) ? dp : 0.f
                };
            } else {
                return _mm_dp_ps(*this, b, mode);
            }
        }
        // dot(x, x) = |x|^2
        template <u8 mode = 0xFF>
        [[nodiscard]] constinl f32x4 dot() const noexcept { return dot<mode>(*this); }

        [[nodiscard]] constinl f32 scl_dot(f32x4 b) const noexcept {
            if consteval { return x*b.x + y*b.y + z*b.z + w*b.w; } else {
                b *= *this;
                b += b.swizzle<2, 3, 0, 0>(); // (xx + zz, yy + ww, ...,  ...)
                b += b.swizzle<1, 0, 0, 0>(); // (xx + zz + yy + ww)
                return b.x;
            }
        }

        [[nodiscard]] constinl f32 scl_dot() const noexcept { return scl_dot(*this); }

        [[nodiscard]] constinl f32 operator[](u32 i) const noexcept { 
            return (&x)[i]; 
        }

        constinl f32x4& operator*=(f32x4 b) noexcept {
            return *this = map<F32SCL(*), _mm_mul_ps>(b);
        }

        // f32x4 * f32x4.rcp() better throughput
        constinl f32x4&  operator/=(f32x4 b) noexcept { 
            return *this = map<F32SCL(/), _mm_div_ps>(b);
        }

        constinl f32x4& operator+=(f32x4 b) noexcept {
            return *this = map<F32SCL(+), _mm_add_ps>(b);
        }

        constinl f32x4& operator-=(f32x4 b) noexcept { 
            return *this = map<F32SCL(-), _mm_sub_ps>(b);
        }

        [[nodiscard]] constinl f32x4 cross(f32x4 b) const noexcept {
            // x = a.y * b.z - b.y * a.z
            // y = a.z * b.x - b.z * a.x
            // z = a.x * b.y - b.x * a.y
            // w = a.w * b.w - a.w * b.w = 0
 
            // [a.y * b.z, a.z * b.x, a.x * b.y] - [a.z * b.y, a.x * b.z, a.y * b.x]

            f32x4 lhs = swizzle<1, 2, 0, 3>() * b.swizzle<2, 0, 1, 3>();
            f32x4 rhs = swizzle<2, 0, 1, 3>() * b.swizzle<1, 2, 0, 3>();

            return lhs - rhs;
        }

        [[nodiscard]] constinl f32x4 unit() const noexcept { 
            return *this * dot().rsqrt();
        }

        //[[nodiscard]] constinl f32x4 min(f32x4 b) const noexcept { return map<std::min, _mm_min_ps>(b); }
        //[[nodiscard]] constinl f32x4 max(f32x4 b) const noexcept { return map<std::max, _mm_max_ps>(b); } 
        [[nodiscard]] constinl f32x4 abs() const noexcept { return andmask<~signbit>(); }
    };

    [[nodiscard]] constinl f32x4 operator-(f32x4 a) noexcept {
        return a.xormask<signbit>();
    }

    [[nodiscard]] constinl f32x4 operator+(f32x4 a, f32x4 b) noexcept { return a += b; }
    [[nodiscard]] constinl f32x4 operator-(f32x4 a, f32x4 b) noexcept { return a -= b; }
    [[nodiscard]] constinl f32x4 operator*(f32x4 a, f32x4 b) noexcept { return a *= b; }
    [[nodiscard]] constinl f32x4 operator/(f32x4 a, f32x4 b) noexcept { return a /= b; }

}

#undef F32BIT
#undef F32SCL