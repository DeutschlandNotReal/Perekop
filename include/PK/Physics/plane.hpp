#pragma once
#include <PK/Math/pose.hpp>

namespace pk {
    namespace Physics {
        struct plane { 
            vec3 pos, nor; 
            plane(vec3 pos, vec3 nor = {0, 1, 0}) noexcept: pos(pos), nor(nor) {}
        
            static plane LookAt(vec3 from, vec3 at) noexcept {
                return {from, normalize(at - from)}; 
            }
        
            float Distance(vec3 point) const noexcept;
            vec3 Project(vec3 point) const noexcept;
        };
    }

    [[nodiscard]] Physics::plane worldspace(Physics::plane, pose) noexcept;
    [[nodiscard]] Physics::plane worldspace(Physics::plane, vec3) noexcept;
    [[nodiscard]] Physics::plane localspace(Physics::plane, pose) noexcept;
    [[nodiscard]] Physics::plane localspace(Physics::plane, vec3) noexcept;
}