#pragma once
#include <random>
#include <PK/Math/pose.hpp>

namespace pk {
    using RNG = std::mt19937;

    template <typename T, typename distribution = std::uniform_real_distribution<T>> 
    T randomval(RNG &rng, T a, T b) noexcept {
        return distribution(a, b)(rng);
    };

    template <int n, typename T, typename distribution = std::uniform_real_distribution<T>>
    vec<n, T> randomvec(RNG &rng, vec<n, T> a, vec<n, T> b) noexcept {
        vec<n, T> res;
        for (int i{0}; i<n; i++) res[i] = randomval<T, distribution>(rng, a[i], b[i]);

        return res;
    }
}