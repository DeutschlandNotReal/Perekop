#pragma once
#include <filesystem>
#include <PK/Math/typedef.hpp>

namespace pk::Render {
    class Texture {
        friend struct Renderer;
        friend class Shader;

        uint index{0};
        int w{0}, h{0};

        public:
            Texture() noexcept = default;
            Texture(const std::filesystem::path&) noexcept;
    };
}