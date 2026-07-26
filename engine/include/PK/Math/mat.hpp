#pragma once
#include <PK/Math/vec.hpp>
#include <utility>

namespace pk {

    template <typename T, u32 n> struct mat {
        using mvec = vec<T, n>;
        mvec val[n];

        [[nodiscard]] constinl mvec operator[](u32 i) const noexcept { return val[i]; }
        [[nodiscard]] constinl mvec& operator[](u32 i) noexcept { return val[i]; }

        [[nodiscard]] constinl mat transpose() const noexcept {
            mat res = *this;
            for (u32 x = 0; x < n; x++) for (u32 y = 0; y < x; y++)
                std::swap(res[x][y], res[y][x]);
            return res;
        }

        inline constexpr mat operator*=(const mat& b) noexcept {
            mat tb = b.transpose();
            mat res;
            for (u32 x = 0; x < n; x++) for (u32 y = 0; y < n; y++)
                res[x][y] = val[x].dot(tb[y]);

            return *this = res;
        }
    };

    template <typename T, u32 n>
    [[nodiscard]] inline constexpr mat<T, n> operator*(mat<T, n> a, const mat<T, n>& b) noexcept { return a *= b; }

    
}