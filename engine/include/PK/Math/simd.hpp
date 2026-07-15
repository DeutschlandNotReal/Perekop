#pragma once
#include <immintrin.h>
#include <bit>
#include <PK/Math/number.hpp>
#include <PK/Core/array.hpp>
#include <cmath>
using std::bit_cast;

#define constinl [[clang::always_inline]] constexpr
#define F32BIT(op) [](f32 x, f32 y){ return bit_cast<f32>(bit_cast<u32>(x) op bit_cast<u32>(y)); }
#define F32SCL(op) [](f32 x, f32 y){ return x op y; }

namespace pk {
    static inline constexpr u32 signbit = 0x80000000;
    // CRTP ( T : simdvec<T> )
    template <typename T>
    struct alignas(16) simdvec {
        public:
            union { __m128 v; struct { f32 x, y, z, w; }; };
            
            constinl simdvec(__m128 v) noexcept: v(v) {}
            
            constinl operator __m128() const noexcept { return v; }

            [[nodiscard]] constinl simdvec(f32 scl = 0.f) noexcept {
                if consteval { x = y = z = w = scl; } else { v = _mm_set1_ps(scl); }
            }

            [[nodiscard]] constinl simdvec(f32 x, f32 y, f32 z, f32 w = 0.f) noexcept: x{x}, y{y}, z{z}, w{w} {}

            constinl operator T() const noexcept { return *this; }

        protected:
            template <auto rt, auto ct, typename... args>
            [[nodiscard]] constinl T map(args... arg) const noexcept {
                if consteval { 
                    return {rt(x, arg.x...), rt(y, arg.y...), rt(z, arg.z...), rt(w, arg.w...)};
                } else { 
                    return ct(*this, arg...); 
                }
            }

            [[nodiscard]] constinl T rcp() const noexcept {
                if consteval {
                    return {1.f/x, 1.f/y, 1.f/z, 1.f/w};
                } else {
                    __m128 y = _mm_rcp_ps(v);

                    // first iteration
                    y = y * (_mm_set1_ps(2.f) - v * y);
                    return y;
                }
            }

            [[nodiscard]] constinl T rsqrt() const noexcept {
                if consteval {
                    f32 res = 1.f / std::sqrtf(x);
                    return {res, res, res, res};
                } else {
                    __m128 y = _mm_rsqrt_ps(v);

                    // first iteration
                    y = y * (_mm_set1_ps(1.5f) - _mm_set1_ps(.5f) * v * y * y);
                    return y;
                }
            }

            [[nodiscard]] constinl T sqrt() const noexcept {
                return rsqrt() * *this;
            }

            template <u8 X, u8 Y, u8 Z, u8 W> 
            [[nodiscard]] constinl T shuffle(const T &b) const noexcept {
                if consteval { 
                    return {b[X], b[Y], b[Z], b[W]}; 
                } else {
                    return _mm_shuffle_ps(*this, b, _MM_SHUFFLE(W, Z, Y, X));
                }
            }

            template <u8 X, u8 Y, u8 Z, u8 W> 
            [[nodiscard]] constinl T swizzle() const noexcept {
                return shuffle<X, Y, Z, W>(*this);
            }

            template <u32 X, u32 Y, u32 Z, u32 W> [[nodiscard]] constinl T andmask() const noexcept { 
                return map<F32BIT(&), _mm_and_ps>(T{bit_cast<f32>(X), bit_cast<f32>(Y), bit_cast<f32>(Z), bit_cast<f32>(W)});
            }

            template <u32 X, u32 Y, u32 Z, u32 W> [[nodiscard]] constinl T xormask() const noexcept { 
                return map<F32BIT(^), _mm_xor_ps>(T{bit_cast<f32>(X), bit_cast<f32>(Y), bit_cast<f32>(Z), bit_cast<f32>(W)});
            }

            template <bool X, bool Y, bool Z, bool W> [[nodiscard]] constinl T negate() const noexcept {
                return xormask<X*signbit, Y*signbit, Z*signbit, W*signbit>();
            }
 
            template <u32 X, u32 Y, u32 Z, u32 W> [[nodiscard]] constinl T ormask() const noexcept { 
                return map<F32BIT(|), _mm_or_ps>(T{bit_cast<f32>(X), bit_cast<f32>(Y), bit_cast<f32>(Z), bit_cast<f32>(W)});
            }

            template <u8 mode = 0xFF> 
            [[nodiscard]] constinl T dot4(T b) const noexcept {
                if consteval {
                    f32 dp = 0;
                    if constexpr (mode & 0x80) dp += x * b.x;
                    if constexpr (mode & 0x40) dp += y * b.y;
                    if constexpr (mode & 0x20) dp += z * b.z;
                    if constexpr (mode & 0x10) dp += w * b.w;

                    return simdvec{
                        (mode & 8) ? dp : 0.f,
                        (mode & 4) ? dp : 0.f,
                        (mode & 2) ? dp : 0.f,
                        (mode & 1) ? dp : 0.f
                    };
                } else {
                    return _mm_dp_ps(*this, b, mode);
                }
            }

            [[nodiscard]] constinl T len4() const noexcept { return dot4(*this).sqrt(); }
            [[nodiscard]] constinl T rlen4() const noexcept { return dot4(*this).rsqrt(); }

        public:
            [[nodiscard]] constinl f32 operator[](u32 i) const noexcept { 
                return (&x)[i]; 
            }
            
            [[nodiscard]] constinl T operator+(T b) const noexcept { 
                return map<F32SCL(+), _mm_add_ps>(b);
            }

            [[nodiscard]] constinl T operator-(T b) const noexcept { 
               return map<F32SCL(-), _mm_sub_ps>(b);
            }

            [[nodiscard]] constinl T operator*(T b) const noexcept { 
                return map<F32SCL(*), _mm_mul_ps>(b);
            }
            
            [[nodiscard]] constinl T operator/(T b) const noexcept { 
                return map<F32SCL(/), _mm_div_ps>(b);
            }

            [[nodiscard]] constinl T operator-() const noexcept {
                return xormask<signbit, signbit, signbit, signbit>();
            }

            constinl T& operator*=(T b) noexcept { return *this = *this * b; }
            constinl T& operator/=(T b) noexcept { return *this = *this / b; }
            constinl T& operator+=(T b) noexcept { return *this = *this + b; }
            constinl T& operator-=(T b) noexcept { return *this = *this - b; }

            [[nodiscard]] constinl T cross(T b) const noexcept {
                // x = a.y * b.z - b.y * a.z
                // y = a.z * b.x - b.z * a.x
                // z = a.x * b.y - b.x * a.y
                // w = a.w * b.w - a.w * b.w = 0
 
                // [a.y * b.z, a.z * b.x, a.x * b.y] - [a.z * b.y, a.x * b.z, a.y * b.x]

                T lhs = swizzle<1, 2, 0, 3>() * b.template swizzle<2, 0, 1, 3>();
                T rhs = swizzle<2, 0, 1, 3>() * b.template swizzle<1, 2, 0, 3>();

                return lhs - rhs;
            }

            [[nodiscard]] constinl f32 dot(T b) const noexcept { return dot4(b).x; }
            [[nodiscard]] constinl T unit() const noexcept { 
                return *this * dot4(*this).rsqrt(); 
            }

            [[nodiscard]] constinl f32 len() const noexcept { return len4().x; }
            [[nodiscard]] constinl f32 rlen() const noexcept { return rlen4().x; }

            [[nodiscard]] constinl T min(T b) const noexcept { return map<std::min, _mm_min_ps>(b); }
            [[nodiscard]] constinl T max(T b) const noexcept { return map<std::max, _mm_max_ps>(b); }
            [[nodiscard]] constinl T abs() const noexcept { return andmask<~signbit, ~signbit, ~signbit, ~signbit>(); }
    };
}

#undef constinl
#undef F32BIT
#undef F32SCL