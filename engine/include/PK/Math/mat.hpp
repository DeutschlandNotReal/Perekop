#pragma once
#include <PK/Math/vec.hpp>
#include <utility>

namespace pk {

    template <typename T, u32 n> struct mat {
        using mvec = vec<T, n>;
        mvec val[n];

        [[nodiscard]] constinl mat() = default;

        template <typename... V> [[nodiscard]] constinl mat(V... vecs) noexcept 
            requires(sizeof...(vecs) == n && (std::is_convertible_v<V, mvec> && ...)) 
        {
            mvec* ptr = val;
            ([&ptr](auto v){
                *(ptr++) = static_cast<mvec>(v);
            } (vecs), ...);
        }

        template <typename... V> [[nodiscard]] constinl mat(V... vals) noexcept 
            requires(sizeof...(vals) == n * n && (std::is_convertible_v<V, T> && ...))
        {
            T* ptr = (T*) val;
            ([&ptr](auto v){
                *(ptr++) = static_cast<T>(v);
            } (vals), ...);
        }

        [[nodiscard]] constinl mvec operator[](u32 i) const noexcept { return val[i]; }
        [[nodiscard]] constinl mvec& operator[](u32 i) noexcept { return val[i]; }

        [[nodiscard]] constinl mat& transpose_inplace() noexcept {
            if constexpr ( n == 4 ) {
                _MM_TRANSPOSE4_PS(val[0], val[1], val[2], val[3]);
            } else {
                for (u32 x = 0; x < n; x++) for (u32 y = x + 1; y < n; y++)
                    std::swap(val[x][y], val[y][x]);
            }

            return *this;
        }

        [[nodiscard]] constinl mat transpose() const noexcept {
            return mat(*this).transpose_inplace();
        }

        inline constexpr mat operator*=(mat b) noexcept {
            b.transpose_inplace();
            mat res;
            for (u32 x = 0; x < n; x++) for (u32 y = 0; y < n; y++)
                res[x][y] = val[x].dot(b[y]);

            return *this = res;
        }
    };

    template <typename T, u32 n>
    [[nodiscard]] inline constexpr mat<T, n> operator*(mat<T, n> a, const mat<T, n>& b) noexcept { return a *= b; }

    template <typename T, u32 n>
    inline constexpr vec<T, n>& operator*=(vec<T, n>& v, const mat<T, n>& m) noexcept {
        vec<T, n> res = v[0] * m[0];
        for (u32 i = 1; i < n; i++) res += v[i] * m[i];
        return v = res;
    }

    template <typename T, u32 n>
    [[nodiscard]] inline constexpr vec<T, n> operator*(vec<T, n> v, const mat<T, n>& m) noexcept {
        return v *= m;
    }

    using mat2 = mat<f32, 2>;
    using mat3 = mat<f32, 3>;
    using mat4 = mat<f32, 4>;
    using imat2 = mat<i32, 2>;
    using imat3 = mat<i32, 3>;
    using imat4 = mat<i32, 4>;
}