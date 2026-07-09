#pragma once
#include <cstring>
#include <type_traits>
#include <utility>
#include <new>
#include <PK/Math/number.hpp>
#include <PK/Core/type.hpp>

#define pk_ainline [[gnu::always_inline]]

namespace pk {
    template <typename T> 
    pk_ainline constexpr const T* ptr_add(const T* ptr, i64 bytes) noexcept {
        return (T*)((const char*)ptr + bytes); 
    }

    template <typename T> 
    pk_ainline constexpr i64 ptr_dif(const T* a, const T* b) noexcept {
        return (i64)b - (i64)a;
    }

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
            const T* end = src + n; 

            while (src < end) new (dst++) T(*src++); 
        } else
            std::memcpy(dst, src, n * sizeof(T));
    }

    template <typename T> 
    inline constexpr void move(T* dst, T* src, u32 n) {
        if (std::is_constant_evaluated() || !std::is_trivially_copyable_v<T>) {
            T* end = src + n; 

            while (src < end) move(dst++, src++);

        } else
            std::memmove(dst, src, n * sizeof(T));
    }

    template <typename T> 
    inline constexpr void rshift(T* src, T* end, u32 n) {
        if (std::is_constant_evaluated() || !std::is_trivially_copyable_v<T>) {
            T* dstend = end + n;

            while (end > src) move(--dstend, --end);
        } else 
            std::memmove(src + n, src, ptr_dif(src, end));
    }

    // returns pointer distance from src to dst
    template <typename T, u32 align>
    inline constexpr i64 realloc(T** data, u32 n, u32 size) {
        T* src = *data;
        T* dst = *data = pk::alloc<T, align>(size);
        i64 dif = ptr_dif(src, dst);
        if (std::is_constant_evaluated() || !std::is_trivially_copyable_v<T>) {
            T* end = src + n;
            while (src < end) move<T, false>(dst++, src++);
        } else {
            std::memmove(dst, src, n * sizeof(T));
        }

        pk::free<align>(src);

        return dif;
    }
}