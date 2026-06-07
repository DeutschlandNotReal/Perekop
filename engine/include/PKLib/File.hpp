#pragma once
#include <PKCore/string.hpp>

namespace pk::File {
    [[nodiscard]] string read(refstring path);
}