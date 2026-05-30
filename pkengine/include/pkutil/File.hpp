#pragma once
#include <pk/Geometry.hpp>

namespace pk::File {
    const char* read(const char* path, bool null_terminated = true, const char** end = nullptr);

}