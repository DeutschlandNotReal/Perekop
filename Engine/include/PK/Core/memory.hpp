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
    constexpr void copy(T* dst, const T* src, uint n = 1) {
        if (std::is_constant_evaluated() || ! std::is_trivially_copyable_v<T>) {
            for (uint i = 0; i < n; i++) std::construct_at(dst + i, src[i]);
        }
        else if (n) std::memcpy(dst, src, n * sizeof(T));
    }

    template <typename T>
    constexpr void move(T* dst, T* src, uint n) {
        if (std::is_constant_evaluated() || !std::is_trivially_copyable_v<T>) {
            if (dst <= src) 
                for (uint i{0}; i < n; ++i) pk::move(dst + i, src + i);
            else
                for (uint i{n}; i-- > 0;) pk::move(dst + i, src + i);
        }
        else if (n) std::memmove(dst, src, n * sizeof(T));
    }

    template <typename T>
    constexpr void rshift(T* src, T* end, uint n) {
        pk::move(src + n, src, uint(end - src));
    }
}