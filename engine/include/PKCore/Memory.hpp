#pragma once
#include <cstring>
#include <type_traits>
#include <cstdint>

#if defined(PK_DEBUG_MEM) && PK_DEBUG_MEM != 0
#include <PKCore/debug.hpp>
// debug flags:
// 0b0001: PKAlloc
// 0b0010: PKFree
// 0b0100: PKCopy
// 0b1000: PKMove
#endif

template <typename T> [[nodiscard]] inline T* PKAlloc(uint32_t n) {
    T* ptr = (T*)::operator new(sizeof(T) * n);
    #if defined(PK_DEBUG_MEM) && (PK_DEBUG_MEM & 0b0001)
        printf("(%s) PKAlloc<%s>(%i) (%p)\n", PK_DEBUG, pk::classname<T>, n, ptr);
    #endif

    return ptr;
}

template <typename T> inline void PKFree(T* ptr) { 
    #if defined(PK_DEBUG_MEM) && (PK_DEBUG_MEM & 0b0010)
        printf("(%s) PKFree<%s> (%p)\n", PK_DEBUG, pk::classname<T>, ptr);
    #endif

    ::operator delete(ptr); 
}

template <typename T> inline void PKCopy(T* dst, const T* src, uint32_t n) {
    #if defined(PK_DEBUG_MEM) && (PK_DEBUG_MEM & 0b0100)
        printf("(%s) PKCopy<%s>(%i) (%p -> %p)\n", PK_DEBUG, pk::classname<T>, n, src, dst);
    #endif

    if (!dst || !src) return;

    if constexpr (std::is_trivially_copyable_v<T>)
        std::memcpy(dst, src, n * sizeof(T));
    else for (int i = 0; i < n; i++)
        new (dst + i) T(src[i]); 
}

template <typename T> inline void PKMove(T* dst, T* src, uint32_t n) {
    #if defined(PK_DEBUG_MEM) && (PK_DEBUG_MEM & 0b1000)
        printf("(%s) PKMove<%s>(%i) (%p -> %p)\n", PK_DEBUG, pk::classname<T>, n, src, dst);
    #endif 

    if (!dst || !src) return;

    if constexpr (std::is_trivially_copyable_v<T>)
        std::memcpy(dst, src, n * sizeof(T));
    else for (int i = 0; i < n; i++)
        new (dst + i) T((T&&) src[i]); 
}