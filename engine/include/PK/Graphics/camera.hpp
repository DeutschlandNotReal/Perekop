#pragma once
#include <PK/Math/transform.hpp>

namespace pk {
    struct Camera { 
        f32 min{.01}, max{200}, fov{70.f};

        vec4 pos;
        quat rot;

        mat4 view() const { 
            return glm::inverse(pose.mat4()); 
        }

        mat4 proj(f32 aspect) const { 
            return glm::perspectiveZO(fov, aspect, min, max); 
        }
    };
}