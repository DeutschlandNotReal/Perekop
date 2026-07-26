#pragma once
#include <PK/Math/number.hpp>
#include <PK/Math/simd.hpp>

namespace pk {    
    template <typename T> T rsqrt(T x) noexcept { 
        return static_cast<T>(1) / std::sqrt(x);
    }

    template <typename T> T rcp(T x) noexcept {
        return static_cast<T>(1) / x;
    }

    template <typename T, u32 n> struct vec;
    template <typename T, u32 n> struct vec_value { T val[n]; };
    template <typename T> struct vec_value<T, 2> { union { T val[2]; struct { T x, y; }; }; };
    template <typename T> struct vec_value<T, 3> { union { T val[3]; struct { T x, y, z; }; }; };
    template <typename T> struct vec_value<T, 4> { union { T val[4]; struct { T x, y, z, w; }; }; };

    template <typename T, u32 n> using vec_pass = std::conditional_t<sizeof(T) * n <= 16, vec<T, n>, const vec<T, n>&>;

    template <typename T, u32 n> struct vec: public vec_value<T, n> {
        #define for_n for (u32 i = 0; i < n; i++) 

        [[nodiscard]] constinl vec() = default;
        [[nodiscard]] constinl vec(T scl) noexcept { for_n this->val[i] = scl; };

        template <typename... arg>
        [[nodiscard]] constinl vec(arg... args) noexcept requires(sizeof...(args) == n) :
            vec_value<T,n>{static_cast<T>(args)...}
        {}

        constinl vec operator+=(vec_pass<T, n> b) noexcept { for_n (*this)[i] += b[i]; }
        constinl vec operator-=(vec_pass<T, n> b) noexcept { for_n (*this)[i] -= b[i]; }
        constinl vec operator*=(vec_pass<T, n> b) noexcept { for_n (*this)[i] *= b[i]; }
        constinl vec operator/=(vec_pass<T, n> b) noexcept { for_n (*this)[i] /= b[i]; }

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

        constinl vec dot(vec b) const noexcept {
            T sum = (*this)[0] * b[0];
            for (u32 i = 1; i < n; i++) sum += (*this)[i] * b[i];
            return sum;
        }
        
        constinl vec unit() const noexcept {
            T rsqrt = static_cast<T>(1) / std::sqrt(dot(*this));
            return *this * rsqrt;
        }
        
        #undef for_n
    };

    // simd specific
    template <> struct vec<f32, 4> {
        union { f32x4 val; struct { f32 x, y, z, w; }; };

        [[nodiscard]] constinl vec() = default;
        [[nodiscard]] constinl vec(f32 scl) noexcept: val(scl) {}
        [[nodiscard]] constinl vec(vec<f32, 3> xyz) noexcept: val(xyz.x, xyz.y, xyz.z, 0.f) {}
        [[nodiscard]] constinl vec(vec<f32, 3> xyz, f32 W) noexcept: val(xyz.x, xyz.y, xyz.z, W) {}
        [[nodiscard]] constinl vec(f32 x, f32 y, f32 z, f32 w) noexcept: x{x}, y{y}, z{z}, w{w} {}
        [[nodiscard]] constinl vec(f32x4 v) noexcept: val{v} {}

        constinl vec operator+=(vec b) noexcept { val += b.val; return *this; }
        constinl vec operator-=(vec b) noexcept { val -= b.val; return *this; }
        constinl vec operator*=(vec b) noexcept { val *= b.val; return *this; }
        constinl vec operator/=(vec b) noexcept { val /= b.val; return *this; }

        
        [[nodiscard]] constinl f32 operator[](u32 i) const noexcept { return (&x)[i]; }
        [[nodiscard]] constinl f32& operator[](u32 i) noexcept { return (&x)[i]; }

        constinl vec cross(vec b) const noexcept {
            return val.cross(b.val);
        }

        constinl vec dot(vec b) const noexcept {
            return val.dot(b.val);
        }

        constinl vec unit() const noexcept {
            return val.unit();
        }
    };

    template <typename T, u32 n> constinl vec<T, n> operator+(vec<T, n> a, vec_pass<T, n> b) noexcept { return a += b; }
    template <typename T, u32 n> constinl vec<T, n> operator-(vec<T, n> a, vec_pass<T, n> b) noexcept { return a -= b; }
    template <typename T, u32 n> constinl vec<T, n> operator*(vec<T, n> a, vec_pass<T, n> b) noexcept { return a *= b; }
    template <typename T, u32 n> constinl vec<T, n> operator/(vec<T, n> a, vec_pass<T, n> b) noexcept { return a /= b; }

    using vec2 = vec<f32, 2>;
    using vec3 = vec<f32, 3>;
    using vec4 = vec<f32, 4>;
    using ivec2 = vec<i32, 2>;
    using ivec3 = vec<i32, 3>;
    using ivec4 = vec<i32, 4>;

    template <typename T, u32 n> constinl vec<T, n> rcp(vec_pass<T, n> v) noexcept {
        vec<T, n> out;
        for (u32 i = 0; i < n; i++) out[i] = rcp(v[i]);
        return out;
    }

    template <> constinl vec4 rcp<f32, 4>(vec4 v) noexcept {
        return v.val.rcp();
    }
}
