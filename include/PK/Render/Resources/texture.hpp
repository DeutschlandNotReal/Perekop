#pragma once
#include <filesystem>

namespace pk::Render {
    class Texture {
        friend struct Renderer;
        friend class Shader;

        unsigned index{0};
        int w{0};
        int h{0};

        public:
            Texture() noexcept = default;
            Texture(const std::filesystem::path&) noexcept;
    };
}