#pragma once
#include <cstring>
#include <type_traits>
#include <utility>
#include <new>
#include <PK/Math/number.hpp>
#include <PK/Core/type.hpp>

namespace pk {
    template <typename T, bool move_destruct = true>   
    constexpr inline void move(T* dst, T* src) {
        if constexpr (std::is_move_constructible_v<T>) {
            new (dst) T(std::move(*src));
            if constexpr (!move_destruct) return;
        } else {
            new (dst) T(*src);
        };
        src->~T();
    }

    template <typename T = char, u32 align = 0> 
    [[nodiscard]] inline constexpr T* alloc(u32 n) {
        if constexpr (align) {
            // sizeof(T) * n must be multiple!!
            u32 bytes = (sizeof(T) * n + align - 1) & ~(align - 1);
            return (T*)::operator new(bytes, std::align_val_t(align));
        } else
            return (T*)::operator new(sizeof(T) * n);
    }

    template <u32 align = 0>
    inline constexpr void free(void *ptr) {
        if constexpr (align)
            ::operator delete(ptr, std::align_val_t(align));
        else 
            ::operator delete(ptr);
    }

    template <typename T> 
    constexpr inline void copy(T* dst, const T* src, u32 n = 1) {
        if (!dst || !src) return;

        if constexpr (std::is_trivially_copyable_v<T> && !std::is_constant_evaluated()) 
            std::memcpy(dst, src, n * sizeof(T));
        else for (int i = 0; i < n; i++)
            new (dst + i) T(src[i]); 
    }

    template <typename T> 
    constexpr inline void move(T* dst, T* src, u32 n) {
        if (!dst || !src) return;

        if constexpr (std::is_trivially_copyable_v<T> && !std::is_constant_evaluated())
            std::memcpy(dst, src, n * sizeof(T));
        else for (int i = 0; i < n; i++) 
            move(dst+i, src+i);
    }

    template <typename T> 
    constexpr inline void rshift(T* src, u32 n) {
        if (!src) return;
        T* dst = src+1;

        if constexpr(std::is_trivially_copyable_v<T> && !std::is_constant_evaluated())
            std::memmove(dst, src, n * sizeof(T));
        else for (int i = n-1; i >= 0; i--)
            move(dst+i, src+i);
    }

    // returns pointer distance from src to dst
    template <typename T, u32 align>
    constexpr inline size_t realloc(T** data, u32 n, u32 size) {
        T* src = *data;
        T* dst = *data = pk::alloc<T, align>(size);
        if constexpr(std::is_trivially_copyable_v<T> && !std::is_constant_evaluated())
            std::memmove(dst, src, n * sizeof(T));
        else for (u32 i = 0; i < n; i++)
            move<T, false>(dst+i, src+i);
        pk::free<align>(src);

        return (size_t)dst - (size_t)src;
    }
}