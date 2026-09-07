#pragma once
#include <fstream>
#include <filesystem>
#include <string>

namespace pk {
    using path = std::filesystem::path;

    inline std::string ReadFile(const path& path) {
        std::ifstream file(path);

        if (!file) return {};

        return std::string(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>()
        );
    }

    inline void WriteFile(const path& path, const void* data, size_t length) {
        std::ofstream file(path, std::ios::binary);

        if (!file) return;

        file.write(
            static_cast<const char*>(data),
            static_cast<std::streamsize>(length)
        );
    }
}