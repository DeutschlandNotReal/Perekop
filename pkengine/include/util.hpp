#pragma once
#include <vector>
#include <functional>

namespace pk::util {
    template <typename T> void swappop(std::vector<T>& V, unsigned int I, std::function<void(T&, unsigned int)> swapf) {
        if ( I != V.size() - 1 ) {
            V[I] = V.back();
            swapf(V[I], I);
        }
        V.pop_back();
    }; 
}