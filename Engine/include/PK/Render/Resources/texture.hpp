#pragma once
#include <filesystem>
#include <PK/Math/typedef.hpp>

namespace pk {
    class texture {
        friend struct renderer;
        friend class shader;

        uint index{0};
        int w{0}, h{0};

        public:
            texture() noexcept = default;
            texture(const std::filesystem::path&) noexcept;
    };
}