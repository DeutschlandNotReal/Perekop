#pragma once
#include <filesystem>
#include <PK/Math/typedef.hpp>

class Texture {
    uint index{0};
    int w{0}, h{0};

    public:
        Texture() noexcept = default;
        Texture(const std::filesystem::path&) noexcept;

        uint getIndex() const noexcept { return index; }

        int getWidth() const noexcept { return w; }
        int getHeight() const noexcept { return h; }

        void generate();
        void allocate(
            int width,
            int height,
            uint internalFormat
        );

        void destroy();
};