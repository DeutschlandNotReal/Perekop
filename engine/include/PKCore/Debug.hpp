#pragma once 
#include <cstdio>
#include <cxxabi.h>

namespace pk {
    template <typename T>
    inline const char* t_name() {
        return abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
    }
}
