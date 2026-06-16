#pragma once
#include <PKCore/vector.hpp>

// searches, sorts and other algorithmns for array
 namespace pk::alg {
    // Linear search
    template <typename T> T* lfind(span<T> array, const T& ref) {
        if (array) for (T *i = array.begin(); i < array.end(); i++)
            if (*i == ref) return i;

        return nullptr;
    }

    // Finds pointer of where ref would be if inserted into array in order, assumes array is already ordered
    template <typename T> T* low_bound(span<T> array, const T &ref) {
        uint32_t l = 0, h = array.size();

        while (l != h) {
            uint32_t m = (l + h) >> 1;
            if (array[m] < ref) l = m+1; else h = m;
        }

        return array.begin() + l;
    }

    // Binary search
    template <typename T> T* bfind(span<T> array, const T &ref, T** nearest = nullptr) {
        T* closest = low_bound(array, ref);
        if (closest < array.end() && *closest == ref)
            return closest;
        else if (nearest) 
            *nearest = closest;

        return nullptr;
    }

    // Inserts item in order, assumes vec is already ordered
    template <typename T> T& ordered_insert(vector<T>& vec, const T &item) {
        if (vec.is_full()) vec.grow();

        T* closest = low_bound(vec, item);

        return vec.push_at(closest - vec.begin());
    }
}