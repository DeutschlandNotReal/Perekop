#pragma once

#include <cxxabi.h>
#include <cstdio>

namespace pk {
    template <typename T>
    inline const char* classname = abi::__cxa_demangle(typeid(T).name(), 0, 0, 0);
}