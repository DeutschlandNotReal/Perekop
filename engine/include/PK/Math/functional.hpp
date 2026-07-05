#pragma once
#include <PK/Math/number.hpp>
#include <PK/Core/array.hpp>

namespace pk {
    template <typename T, u32 n> using pass_array = pass_t<array<T, n>>;

    inline constexpr auto add = [](auto a, auto b) noexcept { return a + b; };
    inline constexpr auto sub = [](auto a, auto b) noexcept { return a - b; };
    inline constexpr auto mul = [](auto a, auto b) noexcept { return a * b; };
    inline constexpr auto div = [](auto a, auto b) noexcept { return a / b; };
    inline constexpr auto neg = [](auto a)         noexcept { return -a; };

    template <typename T, u32 n, auto f> inline constexpr
    T fold(pass_array<T, n> a) noexcept {
        T result = a[0];
        for (u8 i = 1; i < n; i++)
            result = f(result, a[i]);
        return result;
    }

    template <typename T, u32 n, auto f> inline constexpr
    array<T, n> map(pass_array<T, n> a) noexcept {
        array<T, n> result;
        for (u8 i = 0; i < n; i++)
            result[i] = f(a[i]);
        return result;
    }

    template <typename T, u32 n, auto f> inline constexpr
    array<T, n>& map_mut(array<T, n> &a) noexcept {
        for (u8 i = 0; i < n; i++)
            a[i] = f(a[i]);
        return a;
    }

    template <typename T, u32 n, auto f> inline constexpr
    array<T, n> map(pass_array<T, n> a, pass_array<T, n> b) noexcept {
        array<T, n> result;
        for (u8 i = 0; i < n; i++)
            result[i] = f(a[i], b[i]);
        return result;
    }

    template <typename T, u32 n, auto f> inline constexpr
    array<T, n>& map_mut(array<T, n> &a, pass_array<T, n> &b) noexcept {
        for (u8 i = 0; i < n; i++) 
            a[i] = f(a[i], b[i]);
        return a;
    }

    template <typename T, u32 n, auto f> inline constexpr
    array<T, n> map(pass_array<T, n> a, T scl) noexcept {
        array<T, n> result;
        for (u8 i = 0; i < n; i++) 
            result[i] = f(a[i], scl);
        return result;
    }

    template <typename T, u32 n, auto f> inline constexpr
    array<T, n>& map_mut(array<T, n> &a, const T scalar) noexcept {
        for (u8 i = 0; i < n; i++) 
            a[i] = f(a[i], scalar);
        return a;
    }
}