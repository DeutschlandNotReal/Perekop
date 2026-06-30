#pragma once
#include <PK/Core/number.hpp>

namespace pk {
    template <typename T, u32 L> class Arr {
        T data[L];
        public:
            Arr() = default;
            Arr(const T (&arr)[L]): data(arr) {}

            const T* begin() const { return data; }
            const T* end()   const { return data + L; }
            T* begin() { return data; }
            T* end()   { return data + L; }

            const T& operator[](u32 i) const { return data[i]; }
            T& operator[](u32 i) { return data[i]; }

            operator const T*() const { return data; }
            operator T*() { return data; }
    };
}