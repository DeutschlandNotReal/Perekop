#pragma once
#include <PK/Math/pose.hpp>
#include <PK/Math/extra.hpp>

namespace pk {
    struct ray { 
        vec3 pos{0.f}, dir{0.f, 0.f, -1.f};
        vec3 invdir{rcp(dir)};

        ray() noexcept = default;
        ray(vec3 pos, vec3 dir) noexcept: pos{pos}, dir{dir}, invdir{rcp(dir)} {}

        static ray lookAt(vec3 pos, vec3 at) noexcept {
            return {pos, at - pos}; 
        }
    };
    
    [[nodiscard]] ray worldspace(ray, pose) noexcept;
    [[nodiscard]] ray worldspace(ray, vec3) noexcept;
    [[nodiscard]] ray localspace(ray, pose) noexcept;
    [[nodiscard]] ray localspace(ray, vec3) noexcept;
}