#pragma once
#include <immintrin.h>
#include <bit>
#include <PK/Math/number.hpp>
#include <PK/Core/array.hpp>
#include <cmath>
using std::bit_cast;

namespace pk {
    template <typename base>
    struct alignas(16) f32x4 { 
        public:
        union { struct { f32 x, y, z, w; }; __m128 xyzw; };
        private:
        template <auto f, typename... A>
        pk_ainline static constexpr f32x4 map(pass_t<A>... args) noexcept {
            return { f(args.x...), f(args.y...), f(args.z...), f(args.w...) };
        }

        template <auto f, auto simd_f, typename... A>
        pk_ainline static constexpr f32x4 cmap(pass_t<A>... args) noexcept {
            return std::is_constant_evaluated() ? map<f>(args...) : simd_f(args.xyzw...);
        }

        public:
        pk_ainline constexpr f32x4(f32 scl = 0.f) noexcept: x{scl}, y{scl}, z{scl}, w{scl} {};
        pk_ainline constexpr f32x4(const pk::array<f32, 4> &arr) noexcept:
            x{arr[0]}, y{arr[1]}, z{arr[2]}, w{arr[3]} {};

        pk_ainline constexpr f32x4(const f32* ptr) noexcept: xyzw(_mm_loadu_ps(ptr)) {}
        pk_ainline f32x4(const __m128 &xyzw) noexcept: xyzw(xyzw) {};
        pk_ainline operator __m128() const noexcept { return xyzw; }

        pk_ainline constexpr f32 operator[](u32 i) const noexcept { return *(&x + i); }
        
        pk_ainline constexpr f32& operator[](u32 i) noexcept { return *(&x + i); }

        pk_ainline constexpr base operator/(const base &b) const noexcept { return cmap<[](f32 a, f32 b){return a/b;}, _mm_div_ps>(*this, b); }
        pk_ainline constexpr base operator+(const base &b) const noexcept { return cmap<[](f32 a, f32 b){return a+b;}, _mm_add_ps>(*this, b); }
        pk_ainline constexpr base operator-(const base &b) const noexcept { return cmap<[](f32 a, f32 b){return a-b;}, _mm_sub_ps>(*this, b); }
        pk_ainline constexpr base operator&(const base &b) const noexcept { 
            return cmap<[](f32 a, f32 b){
                return bit_cast<f32>(bit_cast<u32>(a)&bit_cast<u32>(b));
            }, _mm_and_ps>(*this, b); 
        }
        pk_ainline constexpr base operator^(const base &b) const noexcept { 
            return cmap<[](f32 a, f32 b){
                return bit_cast<f32>(bit_cast<u32>(a)^bit_cast<u32>(b));
            }, _mm_xor_ps>(*this, b); 
        }
        pk_ainline constexpr base operator|(const base &b) const noexcept { 
            return cmap<[](f32 a, f32 b){
                return bit_cast<f32>(bit_cast<u32>(a)|bit_cast<u32>(b));
            }, _mm_or_ps>(*this, b); 
        }

        pk_ainline constexpr base& operator/=(const base &b) noexcept { return *this = *this / b; }
        pk_ainline constexpr base& operator+=(const base &b) noexcept { return *this = *this + b; }
        pk_ainline constexpr base& operator-=(const base &b) noexcept { return *this = *this - b; }
        pk_ainline constexpr base& operator&=(const base &b) noexcept { return *this = *this & b; }
        pk_ainline constexpr base& operator^=(const base &b) noexcept { return *this = *this ^ b; }
        pk_ainline constexpr base& operator|=(const base &b) noexcept { return *this = *this | b; }

        #ifdef __FMA__
        // for automatic FMA
        class mulexpr {
            friend f32x4;
            base a, scl;
            public:
                pk_ainline constexpr operator f32x4() const noexcept { 
                    return f32x4::cmap<[](f32 a, f32 b){return a*b;}, _mm_mul_ps>(a, scl);
                }
                pk_ainline constexpr operator base() const noexcept { 
                    return f32x4::cmap<[](f32 a, f32 b){return a*b;}, _mm_mul_ps>(a, scl);
                }
                pk_ainline constexpr base operator+(const base &b) const noexcept { 
                    return f32x4::cmap<[](f32 a, f32 b, f32 c){return a*b+c;}, _mm_fmadd_ps>(a, scl, b); 
                }
        };

        pk_ainline constexpr mulexpr operator*(const base &b) const noexcept { return {xyzw, b.xyzw}; }
        pk_ainline constexpr base operator+(const mulexpr &b) const noexcept { return b + *this; }
        #else
        pk_ainline constexpr base operator*(const base &b) const noexcept { return cmap<[](f32 a, f32 b){return a*b;}, _mm_mul_ps>(*this, b); }
        #endif

        pk_ainline constexpr base& operator*=(const base &b) noexcept { return *this = *this * b; }

        pk_ainline constexpr base operator~() const noexcept {
            static constexpr f32x4 xormask{bit_cast<f32>(~0)};
            
            return *this ^ xormask;
        }

        pk_ainline constexpr base dot(const base &b) const noexcept {
            if (std::is_constant_evaluated()) {
                base prod = *this * b;
                f32 dprod = prod.x + prod.y + prod.z + prod.w;
                return {dprod, dprod, dprod, dprod};
            } else {
                return _mm_dp_ps(xyzw, b, 0xFF);
            }
        }

        template <u8 i0, u8 i1, u8 i2, u8 i3> pk_ainline constexpr base shuffle(const base &b) const noexcept {
            static_assert(i0 > 4 || i1 > 4 || i2 > 4 || i3 > 4, "no you don't.");

            if (std::is_constant_evaluated()) 
                return {b[i0], b[i1], b[i2], b[i3]};
            else 
                return _mm_shuffle_ps(xyzw, b, _MM_SHUFFLE(i3, i2, i1, i0));
        }

        template <u8 i0, u8 i1, u8 i2, u8 i3> pk_ainline constexpr base swizzle() const noexcept {
            return shuffle<i0, i1, i2, i3>(*this);
        }

        pk_ainline constexpr base cross(const base &b) const noexcept {
            // x = a.y * b.z - b.y * a.z
            // y = a.z * b.x - b.z * a.x
            // z = a.x * b.y - b.x * a.y

            // [a.y * b.z, a.z * b.x, a.x * b.y] - [a.z * b.y, a.x * b.z, a.y * b.x]

            base lhs = swizzle<1, 2, 0, 3>() * b.template swizzle<2, 0, 1, 3>();
            base rhs = swizzle<2, 0, 1, 3>() * b.template swizzle<1, 2, 0, 3>();

            return lhs - rhs;
        }

        pk_ainline constexpr base sqrt() const noexcept { return cmap<std::sqrtf, _mm_sqrt_ps>(*this); }
        pk_ainline constexpr base isqrt() const noexcept { return cmap<[](f32 a){return 1.f / std::sqrtf(a);}, _mm_rsqrt_ps>(*this); }

        pk_ainline constexpr base unit() const noexcept { return *this * dot(*this).isqrt(); }
        pk_ainline constexpr base min(const base &b) const noexcept { return cmap<std::min, _mm_min_ps>(*this, b); }
        pk_ainline constexpr base max(const base &b) const noexcept { return cmap<std::max, _mm_max_ps>(*this, b); }
        pk_ainline constexpr base abs() const noexcept {
            static constexpr f32x4 negmask{bit_cast<f32>(0x7FFFFFFF)};
            return *this & negmask;
        }
    };
}