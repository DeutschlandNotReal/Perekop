#pragma once
#include <PK/Math/pose.hpp>

namespace pk {
    namespace physics {
        struct plane { 
            vec3 pos, nor; 
            plane(vec3 pos, vec3 nor = {0, 1, 0}) noexcept: pos(pos), nor(nor) {}
        
            static plane lookAt(vec3 from, vec3 at) noexcept {
                return {from, normalize(at - from)}; 
            }
        
            float distance(vec3 point) const noexcept;
            vec3 project(vec3 point) const noexcept;
        };
    }

    [[nodiscard]] physics::plane worldspace(physics::plane, pose) noexcept;
    [[nodiscard]] physics::plane worldspace(physics::plane, vec3) noexcept;
    [[nodiscard]] physics::plane localspace(physics::plane, pose) noexcept;
    [[nodiscard]] physics::plane localspace(physics::plane, vec3) noexcept;
}