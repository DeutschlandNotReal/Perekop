#pragma once

#include <glm/glm.hpp>

namespace pk {
    template <typename T> constexpr T min(T value) noexcept {
        return value;
    }

    template <typename T> constexpr auto min(T first, T second) noexcept {
        return glm::min(first, second);
    }

    template <typename T, typename U, typename... V> constexpr auto min(T first, U second, V... rest) noexcept {
        return min(min(first, second), rest...);
    }

    template <typename T> constexpr T max(T value) noexcept {
        return value;
    }

    template <typename T> constexpr auto max(T first, T second) noexcept {
        return glm::max(first, second);
    }

    template <typename T, typename U, typename... V> constexpr auto max(T first, U second, V... rest) noexcept {
        return max(max(first, second), rest...);
    }
}
