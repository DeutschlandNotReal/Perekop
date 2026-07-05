#pragma once 
#include <type_traits>
 
// trivial types < 16B faster to pass by value, not const-ref
template <typename T> 
using pass_t = std::conditional_t<(sizeof(T) > 16 || !std::is_trivially_copyable_v<T>), const T&, T>;
