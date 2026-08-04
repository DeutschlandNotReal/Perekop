#pragma once
#include <PK/Math/number.hpp>
#include <PK/Math/simd.hpp>
//#include <PK/Core/type.hpp>

namespace pk {    
    template <typename T> inline constexpr T rsqrt(T x) noexcept {
        return static_cast<T>(1) / std::sqrt(x);
    }

    template <typename T> inline constexpr T rcp(T x) noexcept {
        return static_cast<T>(1) / x;
    }

    template <typename T, u32 n> struct vec;
    template <typename T, u32 n> struct vec_value { T val[n]; };
    template <typename T> struct vec_value<T, 2> { union { T val[2]; struct { T x, y; }; }; };
    template <typename T> struct vec_value<T, 3> { union { T val[3]; struct { T x, y, z; }; }; };
    template <typename T> struct vec_value<T, 4> { union { T val[4]; struct { T x, y, z, w; }; }; };

    template <typename T, u32 n> struct vec: public vec_value<T, n> {
        #define for_n for (u32 i = 0; i < n; i++) 

        [[nodiscard]] constinl vec() = default;
        
        template <typename scl_T>
        [[nodiscard]] constinl vec(scl_T scl) noexcept requires(n > 1 && std::is_constructible_v<scl_T, T>) {
             for_n this->val[i] = static_cast<T>(scl); 
        };

        template <typename... arg>
        [[nodiscard]] constinl vec(arg... args) noexcept requires(sizeof...(args) == n && (std::is_convertible_v<arg, T> && ...)) :
            vec_value<T,n>{static_cast<T>(args)...}
        {}

        constinl vec& operator+=(vec b) noexcept { for_n (*this)[i] += b[i]; return *this; }
        constinl vec& operator-=(vec b) noexcept { for_n (*this)[i] -= b[i]; return *this; }
        constinl vec& operator*=(vec b) noexcept { for_n (*this)[i] *= b[i]; return *this; }
        constinl vec& operator/=(vec b) noexcept { for_n (*this)[i] /= b[i]; return *this; }
        constinl vec  operator-() const noexcept { return 0 - *this; }

        [[nodiscard]] constinl T operator[](u32 i) const noexcept { return this->val[i]; }
        [[nodiscard]] constinl T& operator[](u32 i) noexcept { return this->val[i]; }

        constinl vec cross(vec b) const noexcept requires (n == 3) {
            // x = a.y * b.z - b.y * a.z
            // y = a.z * b.x - b.z * a.x
            // z = a.x * b.y - b.x * a.y
            const T x = (*this)[0], y = (*this)[1], z = (*this)[2];

            return {
                y * b.z - b.y * z,
                z * b.x - b.z * x,
                x * b.y - b.x * y
            };
        }

        constinl T dot(vec b) const noexcept {
            T sum = (*this)[0] * b[0];
            for (u32 i = 1; i < n; i++) sum += (*this)[i] * b[i];
            return sum;
        }
        
        constinl vec unit() const noexcept {
            return *this / std::sqrt(dot(*this));
        }
        
        #undef for_n
    };

    // simd specific
    template <> struct vec<f32, 4> {
        union { f32x4 val; struct { f32 x, y, z, w; }; };

        [[nodiscard]] constinl vec() = default;

        template <typename scl_T>
        [[nodiscard]] constinl vec(scl_T scl) noexcept requires(std::is_convertible_v<scl_T, f32>): 
            val(static_cast<f32>(scl)) 
        {}

        template <typename... arg>
        [[nodiscard]] constinl vec(arg... args) noexcept requires(sizeof...(args) == 4 && (std::is_convertible_v<arg, f32> && ...)) :
            val{static_cast<f32>(args)...}
        {}

        [[nodiscard]] constinl vec(f32x4 v) noexcept: val{v} {}

        [[nodiscard]] constinl vec(vec<f32, 3> xyz) noexcept: x{xyz.x}, y{xyz.y}, z{xyz.z}, w{0.f} {}
        [[nodiscard]] constinl operator vec<f32, 3>() noexcept { return {x, y, z}; }

        constinl vec& operator+=(vec b) noexcept { val += b.val; return *this; }
        constinl vec& operator-=(vec b) noexcept { val -= b.val; return *this; }
        constinl vec& operator*=(vec b) noexcept { val *= b.val; return *this; }
        constinl vec& operator/=(vec b) noexcept { val /= b.val; return *this; }
        constinl vec  operator-() const noexcept { return -val; }

        [[nodiscard]] constinl f32 operator[](u32 i) const noexcept { return (&x)[i]; }
        [[nodiscard]] constinl f32& operator[](u32 i) noexcept { return (&x)[i]; }

        constinl vec cross(vec b) const noexcept {
            return val.cross(b.val);
        }

        constinl f32 dot(vec b) const noexcept {
            return val.dot(b.val).x;
        }

        constinl vec unit() const noexcept {
            return val.unit();
        }
    };

    template <typename T, u32 n> constinl vec<T, n> operator+(vec<T, n> a, vec<T, n> b) noexcept { return a += b; }
    template <typename T, u32 n> constinl vec<T, n> operator-(vec<T, n> a, vec<T, n> b) noexcept { return a -= b; }
    template <typename T, u32 n> constinl vec<T, n> operator*(vec<T, n> a, vec<T, n> b) noexcept { return a *= b; }
    template <typename T, u32 n> constinl vec<T, n> operator/(vec<T, n> a, vec<T, n> b) noexcept { return a /= b; }

    #define T_implicit noexcept requires(std::is_convertible_v<scl_T, T>)
    template <typename T, typename scl_T, u32 n> constinl vec<T, n>& operator*=(vec<T, n> a, scl_T b) T_implicit { return a *= vec<T, n>{a}; }
    template <typename T, typename scl_T, u32 n> constinl vec<T, n> operator*(scl_T a, vec<T, n> b) T_implicit { return vec<T, n>{a} * b; }
    template <typename T, typename scl_T, u32 n> constinl vec<T, n> operator*(vec<T, n> a, scl_T b) T_implicit { return b * vec<T, n>{a}; }

    template <typename T, typename scl_T, u32 n> constinl vec<T, n>& operator/=(vec<T, n> a, scl_T b) T_implicit { return a *= rcp(b); }
    template <typename T, typename scl_T, u32 n> constinl vec<T, n> operator/(scl_T a, vec<T, n> b) T_implicit { return vec<T, n>{a} /= b; }
    template <typename T, typename scl_T, u32 n> constinl vec<T, n> operator/(vec<T, n> a, scl_T b) T_implicit { return a /= b; }
    #undef T_implicit

    using vec2 = vec<f32, 2>;
    using vec3 = vec<f32, 3>;
    using vec4 = vec<f32, 4>;
    using ivec2 = vec<i32, 2>;
    using ivec3 = vec<i32, 3>;
    using ivec4 = vec<i32, 4>;

    template <typename T, u32 n> inline constexpr vec<T, n> rcp(vec<T, n> v) noexcept {
        vec<T, n> out;
        for (u32 i = 0; i < n; i++) out[i] = rcp(v[i]);
        return out;
    }

    template <> inline constexpr vec4 rcp<f32, 4>(vec4 v) noexcept {
        return v.val.rcp();
    }

    inline constexpr vec3 from_spherical(f32 xrad, f32 yrad, f32 dst = 1.f) noexcept {
        f32 sinx, cosx, siny, cosy;
        sincosf(xrad, &sinx, &cosx);
        sincosf(yrad, &siny, &cosy);

        return { 
            dst * siny * cosx,
            dst * siny * sinx, 
            dst * cosy         
        };
    }
}
