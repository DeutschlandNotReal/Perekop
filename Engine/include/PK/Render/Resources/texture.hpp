#pragma once
#include <filesystem>
#include "./glresource.hpp"

namespace pk{
    class Texture : public glResource<Texture> {
        int w{0}, h{0};

        public:
            Texture() = default;
            Texture(const std::filesystem::path&) noexcept;

            int getWidth() const noexcept { return w; }
            int getHeight() const noexcept { return h; }

            void allocate(
                int width,
                int height,
                uint internalFormat
            );
    };
}