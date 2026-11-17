#pragma once

#include <glm/glm.hpp>

namespace pk {
    template <typename T, typename... U> constexpr T min(T x, U... y) noexcept {
        return ((x = glm::min(x, y)), ...);
    }

    template <typename T, typename... U> constexpr T max(T x, U... y) noexcept {
        return ((x = glm::max(x, y)), ...);
    }

    template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
    constexpr glm::mat<C, R, T, Q> abs(glm::mat<C, R, T, Q> res) noexcept {
        for (glm::length_t c{0}; c<C; c++) 
            res[c] = glm::abs(res[c]);

        return res;
    }
    
    template <typename T>
    constexpr T rcp(T x) noexcept {
        return static_cast<T>(1) / x;
    }
}
