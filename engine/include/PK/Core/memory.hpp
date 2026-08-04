#pragma once
#include <cstring>
#include <type_traits>
#include <utility>
#include <new>
#include <PK/Math/number.hpp>
#include <PK/Core/type.hpp>
#include <cstdio>

#define PK_MEM_DEBUG true

#define constinl [[clang::always_inline]] constexpr

namespace pk {
    template <typename T, bool destructive = true>
    constinl void move(T* dst, T* src) {
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
    [[nodiscard]] constinl T* alloc(u32 n) {
        // rounded to nearest align
        u32 len = sizeof(T) * n + alignof(T) & ~(alignof(T) - 1);

        T* ptr = (T*)::operator new(len, std::align_val_t(alignof(T)));
        #if defined (PK_MEM_DEBUG) && PK_MEM_DEBUG == true
        printf("PK::ALLOC %ix%iB at %p\n", len, sizeof(T), ptr);
        #endif

        return ptr;
    }

    template <typename T>
    constinl void free(T *ptr) {
        #if defined(PK_MEM_DEBUG) && PK_MEM_DEBUG == true
        printf("PK::FREE %p\n");
        #endif

        ::operator delete(ptr, std::align_val_t(alignof(T)));
    }

    template <typename T> 
    constinl void copy(T* dst, const T* src, u32 n = 1) requires(std::is_copy_constructible_v<T>) {
        if constexpr (std::is_constant_evaluated() || !std::is_trivially_copyable_v<T>) {
            const T* end = src + n; 

            while (src < end) new (dst++) T(*src++); 
        } else
            std::memcpy(dst, src, n * sizeof(T));
    }

    template <typename T, bool destructive = true> 
    constinl void move(T* dst, T* src, u32 n) {
        if constexpr (std::is_constant_evaluated() || !std::is_trivially_copyable_v<T>) {
            T* end = src + n; 

            while (src < end) move<T, destructive>(dst++, src++);

        } else
            std::memmove(dst, src, n * sizeof(T));
    }

    template <typename T> 
    constinl void rshift(T* src, T* end, u32 n) {
        if constexpr (std::is_constant_evaluated() || !std::is_trivially_copyable_v<T>) {
            T* dstend = end + n;

            while (end > src) move(--dstend, --end);
        } else 
            std::memmove(src + n, src, ptr_dif(src, end));
    }
}