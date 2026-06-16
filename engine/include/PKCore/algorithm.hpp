#pragma once
#include <PKCore/vector.hpp>

// searches, sorts and other algorithmns for array
 namespace pk {
    // linear search
    template <typename T> T* lsearch(span<T> array, const T& ref) {
        if (array) for (T *i = array.begin(); i < array.end(); i++)
            if (*i == ref) return i;

        return nullptr;
    }

    // gets pointer where ref would be if sorted
    template <typename T> T* lower_bound(span<T> array, const T &ref) {
        uint32_t l = 0, h = array.size();

        while (l != h) {
            uint32_t m = (l + h) >> 1;
            if (array[m] < ref) l = m+1; else h = m;
        }

        return array.begin() + l;
    }

    // binary search
    template <typename T> T* bsearch(span<T> array, const T &ref, T** nearest = nullptr) {
        T* bound = lower_bound(array, ref);

        if (bound != array.end() && *bound == ref)
            return bound;
        else if (nearest)
            *nearest = bound;

        return nullptr;
    }

    // Inserts item in order, assumes vec is already ordered
    template <typename T> T& ordered_insert(vector<T>& vec, const T &item) {
        if (vec.is_full()) vec.grow();

        T* closest = lower_bound(vec, item);

        return vec.push(closest - vec.begin(), item);
    }
}