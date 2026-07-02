#pragma once
#include <PK/Math/number.hpp>
#include <PK/Core/array.hpp>

namespace pkm {
    template <u8 D, typename T, auto f> inline constexpr
    T fold(const pk::array<T, D> &a) noexcept {
        T result = a[0];
        for (u8 i = 1; i < D; i++)
            result = f(result, a[i]);
        return result;
    }

    template <u8 D, typename T, auto f> inline constexpr
    pk::array<T, D> map(const pk::array<T, D> &a) noexcept {
        pk::array<T, D> result;
        for (u8 i = 0; i < D; i++)
            result[i] = f(a[i]);
        return result;
    }

    template <u8 D, typename T, auto f> inline constexpr
    pk::array<T, D>& map_mut(pk::array<T, D> &a) noexcept {
        for (u8 i = 0; i < D; i++)
            a[i] = f(a[i]);
        return a;
    }

    template <u8 D, typename T, auto f> inline constexpr
    pk::array<T, D> map(const pk::array<T, D> &a, const pk::array<T, D> &b) noexcept {
        pk::array<T, D> result;
        for (u8 i = 0; i < D; i++)
            result[i] = f(a[i], b[i]);
        return result;
    }

    template <u8 D, typename T, auto f> inline constexpr
    pk::array<T, D>& map_mut(pk::array<T, D> &a, const pk::array<T, D> &b) noexcept {
        for (u8 i = 0; i < D; i++) 
            a[i] = f(a[i], b[i]);
        return a;
    }

    template <u8 D, typename T, auto f> inline constexpr
    pk::array<T, D> map(const pk::array<T, D> &a, T scalar) noexcept {
        pk::array<T, D> result;
        for (u8 i = 0; i < D; i++) 
            result[i] = f(a[i], scalar);
        return result;
    }

    template <u8 D, typename T, auto f> inline constexpr
    pk::array<T, D>& map_mut(pk::array<T, D> &a, const T scalar) noexcept {
        for (u8 i = 0; i < D; i++) 
            a[i] = f(a[i], scalar);
        return a;
    }
}