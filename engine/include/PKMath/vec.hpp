#pragma once
#include <PKMath/functional.hpp>
#include <PKMath/scalar.hpp>

namespace pkm {
    inline constexpr auto add = [](auto a, auto b){ return a + b; };
    inline constexpr auto sub = [](auto a, auto b){ return a - b; };
    inline constexpr auto mul = [](auto a, auto b){ return a * b; };
    inline constexpr auto div = [](auto a, auto b){ return a / b; };
    inline constexpr auto neg = [](auto a){ return -a; };

    template <u8 D, typename T> T dot(const T (&a)[D], const T (&b)[D]) noexcept {
        T result = a[0] * b[0];
        for (u8 i = 1; i < D; i++)
            result += a[i] * b[i];
        return result;
    }

    template <u8 D, typename T> union vec_value { T v[D]; };
    template <typename T> union vec_value<2, T> { T v[2]; struct { T x, y; }; };
    template <typename T> union vec_value<3, T> { T v[3]; struct { T x, y, z; }; };
    template <typename T> union vec_value<4, T> { T v[4]; struct { T x, y, z, w; }; };

    template <u8 D, typename T> struct vec: public vec_value<D, T> {
        using vec_value<D, T>::v;

        T& operator[](u8 i) noexcept { return v; }
        T operator[](u8 i) const noexcept { return v; }

        template <typename t> vec<D, T> operator*(t b) const noexcept {
            return pkm::map<D, T, mul>(v, b);
        }

        template <typename t> vec<D, T>& operator*=(t b) noexcept {
            return pkm::map_mut<D, T, mul>(v, b);
        }

        template <typename t> vec<D, T> operator+(t b) const noexcept {
            return pkm::map<D, T, add>(v, b);
        }
            
        template <typename t> vec<D, T>& operator+=(t b) noexcept {
            return pkm::map_mut<D, T, add>(v, b);
        }

        template <typename t> vec<D, T> operator-(t b) const noexcept {
            return pkm::map<D, T, sub>(v, b);
        }

        vec<D, T> operator-() const noexcept {
            return pkm::map<D, T, neg>(v);
        }
            
        template <typename t> vec<D, T>& operator-=(t b) noexcept {
            return pkm::map_mut<D, T, sub>(v, b);
        }
            
        template <typename t> vec<D, T> operator/(t b) const noexcept {
            return pkm::map<D, T, div>(v, b);
        }
            
        template <typename t> vec<D, T>& operator/=(t b) noexcept {
            return pkm::map_mut<D, T, div>(v, b);
        }

        T mag2() const noexcept { 
            return dot(v, v); 
        }

        T mag() const noexcept { 
            return sqrt(mag2()); 
        }

        vec<D, T> unit() const noexcept { return *this * isqrt(mag2()); }
        vec<D, T>& norm() noexcept { return *this *= isqrt(mag2()); }
    };

    using vec2 = vec<2, f32>;
    using vec3 = vec<3, f32>;
    using vec4 = vec<4, f32>;

    using ivec2 = vec<2, i32>;
    using ivec3 = vec<3, i32>;
    using ivec4 = vec<4, i32>;

    using uvec2 = vec<2, u32>;
    using uvec3 = vec<3, u32>;
    using uvec4 = vec<4, u32>;
}