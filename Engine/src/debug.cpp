#include <PK/debug.hpp>
#include <print>

namespace colours {
    auto constexpr warn = 33;    // yellow
    auto constexpr white = 37; 
    auto constexpr error = 31;   // red
    auto constexpr success = "38;5;114"; // green
}

void pk::warn(std::string_view text, std::source_location loc) noexcept {
    std::print("\033[{}m[{}:{}]: {} \033[{}m\n", colours::warn, loc.file_name(), loc.line(), text, colours::white);
}

void pk::error(const std::exception &E, std::string_view text, std::source_location loc) {
    std::print("\033[{}m[{}:{}] [{}]: {} \033[{}m\n", colours::error, loc.file_name(), loc.line(), E.what(), text, colours::white);
    throw;
}

void pk::success(std::string_view text, std::source_location loc) noexcept {
    std::print("\033[{}m[{}:{}]: {} \033[{}m\n", colours::success, loc.file_name(), loc.line(), text, colours::white);
}