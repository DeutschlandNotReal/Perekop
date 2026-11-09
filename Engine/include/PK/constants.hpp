#pragma once
#include <PK/Math/typedef.hpp>
#include <limits>

// holy keyword bloat

namespace pk {
    static inline constexpr uint nullid = -1;

    template <typename T> using inf = std::numeric_limits<T>::infinity();
    static inline constexpr float epsilon = 1e-12;
}