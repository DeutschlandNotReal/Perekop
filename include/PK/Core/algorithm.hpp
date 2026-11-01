#pragma once
#include <PK/Core/span.hpp>

namespace pk {
    template <typename T, auto f = T::operator<, typename... args> void insertsort(span<T> &data, args&& ...arg) noexcept {
        if (data.size() < 2) return;


        for (T* i{data.begin()+1}; i < data.end(); i++) {
            T val{std::move(*i)};
            T* j{i-1};

            while (j >= data.begin() && f(*j, val, std::forward<args>(arg)...))
                *(j+1) = std::move(*j--);

            *(j+1) = std::move(val);
        }
    }

    
}