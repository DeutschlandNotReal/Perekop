#pragma once
#include <cstring>
#include <type_traits>
#include <PK/Core/number.hpp>

namespace pk {
    template <typename T> constexpr std::remove_reference_t<T>&& rvalue_cast(T&& v) noexcept {
        return static_cast<std::remove_reference_t<T>&&>(v);
    }

    template <typename T> constexpr T&& forward_cast(std::remove_reference_t<T>& t) noexcept {
        return static_cast<T&&>(t);
    }

    template <typename T> constexpr T&& forward_cast(std::remove_reference_t<T>&& t) noexcept {
        return static_cast<T&&>(t);
    }

    // for types <16B, passing by value is better than by const-ref (as long as trivially copyable)
    template <typename T> using constref = std::conditional_t<(sizeof(T) > 16 || !std::is_trivially_copyable_v<T>), const T&, T>;

    // just to make it blue like the other casts
    #define rvalue_cast rvalue_cast
    #define forward_cast forward_cast

    template <typename T> inline void move(T* dst, T* src) {
        if constexpr (std::is_move_constructible_v<T>)
            new (dst) T(rvalue_cast(*src));
        else
            new (dst) T(*src);
        src->~T();
    }

    template <typename T = char> [[nodiscard]] inline T* alloc(u32 n) {
        if (!n) return nullptr;
        
        return (T*)::operator new(n * sizeof(T));
    }

    inline void free(void* ptr) { ::operator delete(ptr); }

    template <typename T> inline void copy(T* dst, const T* src, u32 n = 1) {
        if (!dst || !src) return;

        if constexpr (std::is_trivially_copyable_v<T>)
            std::memcpy(dst, src, n * sizeof(T));
        else for (int i = 0; i < n; i++)
            new (dst + i) T(src[i]); 
    }

    template <typename T> inline void move(T* dst, T* src, u32 n) {
        if (!dst || !src) return;

        if constexpr (std::is_trivially_copyable_v<T>)
            std::memcpy(dst, src, n * sizeof(T));
        else for (int i = 0; i < n; i++) 
            move(dst+i, src+i);
    }

    template <typename T> inline void rshift(T* src, u32 n) {
        if (!src) return;
        T* dst = src+1;

        if constexpr(std::is_trivially_copyable_v<T>) {
            std::memmove(dst, src, n * sizeof(T));
        } else for (int i = n-1; i >= 0; i--)
            move(dst+i, src+i);
    }
}