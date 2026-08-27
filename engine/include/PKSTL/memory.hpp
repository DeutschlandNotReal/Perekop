#pragma once
#include <cstring>
#include <utility>
#include <new>

namespace pk {
    template <typename T, bool destructive = true>
    constexpr void move(T* dst, T* src) {
        // non-trivial path
        if constexpr (std::is_move_constructible_v<T>) {
            new (dst) T(std::move(*src));
            if constexpr (!destructive) return;
        } else {
            new (dst) T(*src);
        }

        src->~T();
    }

    template <typename T = char>
    [[nodiscard]] constexpr T* alloc(unsigned n) {
        // rounded to nearest align
        unsigned len = sizeof(T) * n + alignof(T) & ~(alignof(T) - 1);

        T* ptr = (T*)::operator new(len, std::align_val_t(alignof(T)));
        return ptr;
    }

    template <typename T>
    constexpr void free(T *ptr) {
        ::operator delete(ptr, std::align_val_t(alignof(T)));
    }

    template <typename T> 
    constexpr void copy(T* dst, const T* src, unsigned n = 1) requires(std::is_copy_constructible_v<T>) {
        if constexpr (std::is_constant_evaluated() || !std::is_trivially_copyable_v<T>) {
            const T* end = src + n; 

            while (src < end) new (dst++) T(*src++); 
        } else
            std::memcpy(dst, src, n * sizeof(T));
    }

    template <typename T, bool destructive = true> 
    constexpr void move(T* dst, T* src, unsigned n) {
        if constexpr (std::is_constant_evaluated() || !std::is_trivially_copyable_v<T>) {
            T* end = src + n; 

            while (src < end) move<T, destructive>(dst++, src++);

        } else
            std::memmove(dst, src, n * sizeof(T));
    }

    template <typename T> 
    constexpr void rshift(T* src, T* end, unsigned n) {
        if constexpr (std::is_constant_evaluated() || !std::is_trivially_copyable_v<T>) {
            T* dstend = end + n;

            while (end > src) move(--dstend, --end);
        } else 
            std::memmove(src + n, src, ptr_dif(src, end));
    }
}