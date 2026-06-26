#pragma once
#include <PKMath/number.hpp>

namespace pkm {
    template <typename T> inline constexpr T pi;
    template <> inline constexpr f64 pi<f64> = 3.14159265358979323846;
    template <> inline constexpr f32 pi<f32> = 3.1415926535f;

    template <typename T> inline constexpr T e;
    template <> inline constexpr f64 e<f64> = 2.71828182845904523536;
    template <> inline constexpr f32 e<f32> = 2.7182818284f;

    template <typename T> inline constexpr T phi;
    template <> inline constexpr f64 phi<f64> = 1.61803398874989484820;
    template <> inline constexpr f32 phi<f32> = 1.6180339887f;

    template <typename T> inline constexpr T sqrt2;
    template <> inline constexpr f64 sqrt2<f64> = 1.41421356237309504880;
    template <> inline constexpr f32 sqrt2<f32> = 1.4142135623f;

    template <typename T> inline constexpr T ln2;
    template <> inline constexpr f64 ln2<f64> = 0.69314718055994530941;
    template <> inline constexpr f32 ln2<f32> = 0.6931471805f;
}