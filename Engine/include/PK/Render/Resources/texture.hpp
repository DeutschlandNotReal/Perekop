#pragma once
#include <filesystem>
#include <PK/Math/typedef.hpp>

namespace pk {
    class Texture {
        friend struct Renderer;
        friend class shader;

        uint index{0};
        int w{0}, h{0};

        public:
            Texture() noexcept = default;
            Texture(const std::filesystem::path&) noexcept;
    };
}