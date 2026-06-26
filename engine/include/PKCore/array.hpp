#pragma once
#include <PKCore/memory.hpp>

namespace pk {
    template <typename T, u32 L = -1> class array {
        T data[L];
        public:
            array() = default;

            T* begin() { return data; }
            T* end()   { return data + L; }
            T& operator[](u32 i) { return data[i]; }

            operator T*()  { return data; }
    };

    // on heap
    template <typename T> class array<T, (u32)-1> {
        T* data; u32 L;
        public:
            array(u32 L): data(pk::alloc<T>(L)), L(L) {}
            ~array() { pk::free(data); }

            T* begin() const { return data; }
            T* end() const { return data + L; }
            T& operator[](u32 i) { return data[i]; }

            operator T*() { return data; }

            template <u32 l> array(const array<T, l>& arr): L(l), data(pk::alloc<T>(l)) {
                pk::copy(data, arr, l);
            }
    };
}