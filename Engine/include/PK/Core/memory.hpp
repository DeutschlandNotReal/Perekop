#pragma once

#include <PK/Math/typedef.hpp>

#include <cstddef>
#include <cstring>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

namespace pk {
    template <typename T, typename U = T>
    constexpr T exchange(T& y, U&& replace) {
        T old = std::move(y);
        y = std::forward<U>(replace);
        return old;
    }

    template <typename T, typename U = T>
    constexpr void exchange(T& x, T& y, U&& replace) {
        x = std::move(y);
        y = std::forward<U>(replace);
    }

    template <typename T>
    constexpr void move(T* dst, T* src) {
        if (dst == src) return;
        std::construct_at(dst, std::move(*src));
        std::destroy_at(src);
    }

    template <typename T = char>
    [[nodiscard]] T* alloc(uint n) {
        return static_cast<T*>(::operator new(sizeof(T) * n, std::align_val_t(alignof(T))));
    }

    template <typename T>
    void free(T* ptr) {
        ::operator delete(ptr, std::align_val_t(alignof(T)));
    }

    template <typename T>
    inline constexpr bool relocatable =
        std::is_trivially_copyable_v<T> && std::is_trivially_destructible_v<T>;

    template <typename T>
    constexpr void copy(T* dst, const T* src, uint n = 1) {
        if constexpr (std::is_trivially_copyable_v<T>) {
            if (!std::is_constant_evaluated()) {
                if (n) std::memcpy(dst, src, n * sizeof(T));
                return;
            }
        }
        for (uint i{0}; i < n; ++i) std::construct_at(dst + i, src[i]);
    }

    template <typename T>
    constexpr void move(T* dst, T* src, uint n) {
        if constexpr (relocatable<T>) {
            if (!std::is_constant_evaluated()) {
                if (n) std::memmove(dst, src, n * sizeof(T));
                return;
            }
        }
        if (dst == src) return;

        if (dst < src)
            for (uint i{0}; i < n; ++i) pk::move(dst + i, src + i);
        else
            for (uint i{n}; i-- > 0;) pk::move(dst + i, src + i);
    }

    template <typename T>
    constexpr void rshift(T* src, T* end, uint n) {
        pk::move(src + n, src, uint(end - src));
    }
}