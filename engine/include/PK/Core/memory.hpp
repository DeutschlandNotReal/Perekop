#pragma once
#include <cstring>
#include <type_traits>
#include <utility>
#include <new>
#include <PK/Math/number.hpp>
#include <PK/Core/type.hpp>

#define pk_ainline [[gnu::always_inline]]
namespace pk {
    template <typename T, bool destructive = true>
    pk_ainline constexpr void move(T* dst, T* src) {
        if constexpr (std::is_move_constructible_v<T>) {
            new (dst) T(std::move(*src));
            if constexpr (!destructive) return;
        } else {
            new (dst) T(*src);
        }

        src->~T();
    }

    template <typename T = char, u32 align = 0> 
    [[nodiscard]] inline constexpr T* alloc(u32 n) {
        if constexpr (align) {
            // sizeof(T) * n must be multiple!!
            u32 bytes = (sizeof(T) * n + align - 1) & ~(align - 1);
            return (T*)::operator new(bytes, std::align_val_t(align));
        } else
            return (T*)::operator new(n * sizeof(T));
    }

    template <u32 align = 0>
    inline constexpr void free(void *ptr) {
        if constexpr (align)
            ::operator delete(ptr, std::align_val_t(align));
        else
            ::operator delete(ptr);
    }

    template <typename T> 
    inline constexpr void copy(T* dst, const T* src, u32 n = 1) {
        if (std::is_constant_evaluated() || !std::is_trivially_copyable_v<T>) {
            T* end = src + n; 
            i64 dif = (i64)dst - (i64)src;

            while (src < end)
                new (((u8*)src + dif)) T(*src++); 

        } else
            std::memcpy(dst, src, n * sizeof(T));
    }

    template <typename T> 
    inline constexpr void move(T* dst, T* src, u32 n) {
        if (std::is_constant_evaluated() || !std::is_trivially_copyable_v<T>) {
            T* end = src + n; 
            i64 dif = (i64)dst - (i64)src;

            while (src < end)
                new (((u8*)src + dif)) T(move(*src++));
        } else
            std::memmove(dst, src, n * sizeof(T));
    }

    template <typename T> 
    inline constexpr void rshift(T* src, T* end, u32 n) {
        if (std::is_constant_evaluated() || !std::is_trivially_copyable_v<T>) {
            u64 offset = n * sizeof(T);
            while (--end >= src)
                move((T*) ((u8*)end + offset));
        } else 
            std::memmove(src + n, src, end - src);
    }

    // returns pointer distance from src to dst
    template <typename T, u32 align>
    inline constexpr i64 realloc(T** data, u32 n, u32 size) {
        T* src = *data;
        T* dst = *data = pk::alloc<T, align>(size);
        i64 dif = (i64)dst - (i64)src;

        if (std::is_constant_evaluated() || !std::is_trivially_copyable_v<T>) {
            T* end = src + n;
            while (src < end) move<T, false>((T*) ((char*)src + dif), src++);
        } else {
            std::memmove(dst, src, n * sizeof(T));
        }

        pk::free<align>(src);

        return dif;
    }
}