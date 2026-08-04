#pragma once
#include <type_traits>

namespace pk {
    // if trivial and size < 8B, better to pass by value than const-ref
    template <typename T> 
    using pass_t = typename std::conditional<std::is_trivially_copyable_v<T> && sizeof(T) <= 8, T, const T&>::type;


}