#pragma once
#include <cstring>
#include <type_traits>
#include <cstdint>

#ifdef PK_DEBUG
#define PK_DEBUG_MEM PK_DEBUG
#endif

#ifdef PK_DEBUG_MEM
#include <PKCore/Debug.hpp>
#endif

template <typename T> [[nodiscard]] inline T* PKAlloc(uint32_t n) {
    T* ptr = (T*)::operator new(sizeof(T) * n);
    #ifdef PK_DEBUG_MEM
    printf("(%s) PKAlloc<%s>(%i) (%p)\n", PK_DEBUG_MEM, pk::t_name<T>(), n, ptr);
    #endif

    return ptr;
}

template <typename T> inline void PKFree(T* ptr) { 
    #ifdef PK_DEBUG_MEM
    printf("(%s) PKFree<%s> (%p)\n", PK_DEBUG_MEM, pk::t_name<T>(), ptr);
    #endif

    ::operator delete(ptr); 
}

template <typename T> inline void PKCopy(T* dst, const T* src, uint32_t n) {
    #ifdef PK_DEBUG_MEM
    printf("(%s) PKCopy<%s>(%i) (%p -> %p)\n", PK_DEBUG_MEM, pk::t_name<T>(), n, src, dst);
    #endif

    if (!dst || !src) return;

    if constexpr (std::is_trivially_copyable_v<T>)
        std::memcpy(dst, src, n * sizeof(T));
    else for (int i = 0; i < n; i++)
        new (dst + i) T(src[i]); 
}

template <typename T> inline void PKMove(T* dst, T* src, uint32_t n) {
    #ifdef PK_DEBUG_MEM
    printf("(%s) PKMove<%s>(%i) (%p -> %p)\n", PK_DEBUG_MEM, pk::t_name<T>(), n, src, dst);
    #endif 
    if (!dst || !src) return;

    if constexpr (std::is_trivially_copyable_v<T>)
        std::memcpy(dst, src, n * sizeof(T));
    else for (int i = 0; i < n; i++)
        new (dst + i) T((T&&) src[i]); 
}