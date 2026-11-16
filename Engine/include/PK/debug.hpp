#pragma once
#include <print>
#include <source_location>

// remember for later vvv
// gdb ./Perekop.exe
// -> run
// -> bt
// -> bt full

namespace pk {
    void warn(std::string_view text, std::source_location loc = std::source_location::current()) noexcept;
    void error(const std::exception&, std::string_view text = "", std::source_location loc = std::source_location::current());
    void success(std::string_view text, std::source_location loc = std::source_location::current()) noexcept;
}