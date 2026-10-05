#pragma once
#include <PK/Math/pose.hpp>

namespace pk {
    struct ray { 
        vec3 pos, dir;
        ray() noexcept: pos{0}, dir{0,0,-1} {}
        ray(vec3 pos, vec3 dir) noexcept: pos{pos}, dir{dir} {}
        ray(pose pose) noexcept: pos{pose.pos}, dir{pose.Forward()} {}

        static ray LookAt(vec3 pos, vec3 at) noexcept {
            return {pos, at - pos}; 
        }

        // values in local space
        float IntersectCube(vec3 ext, vec3 localpos) const noexcept;
        float IntersectBall(float r, vec3 localpos) const noexcept;
    };
    
    [[nodiscard]] ray worldspace(ray, pose) noexcept;
    [[nodiscard]] ray worldspace(ray, vec3) noexcept;
    [[nodiscard]] ray localspace(ray, pose) noexcept;
    [[nodiscard]] ray localspace(ray, vec3) noexcept;
}