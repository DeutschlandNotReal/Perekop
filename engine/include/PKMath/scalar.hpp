#pragma once
#include <PKMath/number.hpp>
#include <PKMath/constants.hpp>
#include <cmath>

#define genf template <typename T> inline constexpr
#define varf(...) template <typename T, __VA_ARGS__> inline constexpr
#define spef template <> inline constexpr

namespace pkm {
    genf T sqrt(T x)       noexcept { return std::sqrt(x); }
    genf T isqrt(T x)      noexcept { return (T)1 / sqrt(x); }
    genf T exp(T x)        noexcept { return std::expf(x); }
    genf T ln(T x)         noexcept { return std::logf(x); }
    genf T log(T x, T b)   noexcept { return ln(x) / ln(b); }
    varf(T b) T log(T x)   noexcept { return ln(x) * ((T)1 / ln(b)); }

    genf T sin(T x)        noexcept { return std::sin(x); }
    genf T cos(T x)        noexcept { return std::cos(x); }
    genf T tan(T x)        noexcept { return std::tan(x); }
    genf T tanh(T x)       noexcept { return std::tanh(x); }
    genf T sinh(T x)       noexcept { return std::sinh(x); }
    genf T cosh(T x)       noexcept { return std::cosh(x); }
    genf T asin(T x)       noexcept { return std::asin(x); }
    genf T acos(T x)       noexcept { return std::acos(x); }
    genf T atan(T x)       noexcept { return std::atan(x); }
    genf T atan2(T x, T y) noexcept { return std::atan2(y, x); } 
}

#undef genf
#undef varf
#undef spef