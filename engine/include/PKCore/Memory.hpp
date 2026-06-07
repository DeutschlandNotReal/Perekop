#pragma once
#include <cstring>
#include <type_traits>
#include <cstdint>

template <typename T = char> [[nodiscard]] inline T* PKAlloc(uint32_t n) {
    return (T*)::operator new(sizeof(T)*n);
}

inline void PKFree(void* ptr) { 
    ::operator delete(ptr); 
}

template <typename T> inline void PKCopy(T* dst, const T* src, uint32_t n = 1) {
    if (!dst || !src) return;

    if constexpr (std::is_trivially_copyable_v<T>)
        std::memcpy(dst, src, n * sizeof(T));
    else for (int i = 0; i < n; i++)
        new (dst + i) T(src[i]); 
}

template <typename T> inline void PKMove(T* dst, T* src, uint32_t n = 1) {
    if (!dst || !src) return;

    if constexpr (std::is_trivially_copyable_v<T>)
        std::memcpy(dst, src, n * sizeof(T));
    else for (int i = 0; i < n; i++)
        new (dst + i) T((T&&) src[i]); 
}