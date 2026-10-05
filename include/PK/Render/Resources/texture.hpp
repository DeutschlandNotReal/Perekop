#pragma once
#include <filesystem>

namespace pk::Render {
    class Texture {
        friend struct Renderer;
        friend class Shader;
        friend class Framebuffer;

        unsigned index{0};
        int w{0};
        int h{0};

        public:
            Texture() noexcept = default;
            Texture(const std::filesystem::path&) noexcept;

            static Texture Colour(int width, int height) noexcept;
            static Texture Depth(int width, int height, bool compare = true) noexcept;
    };
}