#pragma once
#include <PKMath/vec.hpp>

namespace pkm {
    template <u8 dx, u8 dy, typename T> class mat {
        vec<dy, T> cols[dx];
        public:
            mat() = default;
            mat(T scl);

            const vec<dy, T>& operator[](u8 i) const { return cols[i]; }
            vec<dy, T>& operator[](u8 i) { return cols[i]; }

            mat<dx, dy, T> operator+(const mat<dx, dy, T>& b) const noexcept {
                return map<dy, T, add>(cols, b.cols);
            }

            mat<dx, dy, T>& operator+=(const mat<dx, dy, T>& b) noexcept {
                return map_mut<dy, T, add>(cols, b.cols);
            }

            mat<dx, dy, T> operator-(const mat<dx, dy, T>& b) const noexcept {
                return map<dy, T, sub>(cols, b.cols);
            }

            mat<dx, dy, T>& operator-=(const mat<dx, dy, T>& b) noexcept {
                return map_mut<dy, T, sub>(cols, b.cols);
            }

            mat<dx, dy, T> operator*(T scalar) const noexcept {
                return map<dy, T, mul>(cols, scalar);
            }

            mat<dx, dy, T>& operator*=(T scalar) noexcept {
                return map_mut<dy, T, mul>(cols, scalar);
            }

            mat<dx, dy, T> operator/(T scalar) const noexcept {
                return map<dy, T, div>(cols, scalar);
            }

            mat<dx, dy, T>& operator/=(T scalar) noexcept {
                return map_mut<dy, T, div>(cols, scalar);
            }

    };

    using mat2 = mat<2, 2, f32>;
    using mat3 = mat<3, 3, f32>;
    using mat4 = mat<4, 4, f32>;
}