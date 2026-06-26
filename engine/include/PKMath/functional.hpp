#pragma once
#include <PKMath/number.hpp>
#include <PKCore/array.hpp>

// in O3 these should automatically be vectorized (SIMD!!)

#define f_func template <u8 D, typename T, auto f> inline constexpr
namespace pkm {
    f_func T fold(const T(&a)[D]) noexcept {
        T result = a[0];
        for (u8 i = 1; i < D; i++)
            result = f(result, a[i]);
        return result;
    }

    f_func pk::array<T, D> map(const T(&a)[D]) noexcept {
        pk::array<T, D> result;
        for (u8 i = 0; i < D; i++)
            result[i] = f(a[i]);
        return result;
    }

    f_func pk::array<T, D>& map_mut(const T(&a)[D]) noexcept {
        for (u8 i = 0; i < D; i++)
            a[i] = f(a[i]);
        return a;
    }

    f_func pk::array<T, D> map(const T(&a)[D], const T(&b)[D]) noexcept {
        pk::array<T, D> result;
        for (u8 i = 0; i < D; i++)
            result[i] = f(a[i], b[i]);
        return result;
    }

    f_func pk::array<T, D>& map_mut(T(&a)[D], const T(&b)[D]) noexcept {
        for (u8 i = 0; i < D; i++) 
            a[i] = f(a[i], b[i]);
        return a;
    }

    f_func pk::array<T, D> map(const T(&a)[D], T scalar) noexcept {
        pk::array<T, D> result;
        for (u8 i = 0; i < D; i++) 
            result[i] = f(a[i], scalar);
        return result;
    }

    f_func pk::array<T, D>& map_mut(T(&a)[D], T scalar) noexcept {
        for (u8 i = 0; i < D; i++) 
            a[i] = f(a[i], scalar);
        return a;
    }
}
#undef f_func