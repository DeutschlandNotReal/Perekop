#pragma once

#include <PK/Core/span.hpp>

#include <functional>

namespace pk {
    template <typename T, auto f = T::operator>, typename... args> void insertsort(span<T> &data, args&& ...arg) noexcept {
        if (data.size() < 2) return;

        for (T* i{data.begin()+1}; i < data.end(); i++) {
            T val{std::move(*i)};
            T* j{i};

            while (j > data.begin() && std::invoke(f, *(j-1), val, std::forward<args>(arg)...)) {
                *j = std::move(*(j-1));
                --j;
            }

            *j = std::move(val);
        }
    } 
}