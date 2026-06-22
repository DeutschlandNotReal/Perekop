#pragma once
#include <PKLib/math.hpp>
#include <PKLib/pose.hpp>

namespace pk {
    struct Camera { 
        float min{.01}, max{200}, fov{glm::radians(70.f)}; 
        Pose pose;

        mat4 view() const { 
            return glm::inverse(pose.mat4()); 
        }

        mat4 proj(float aspect) const { 
            return glm::perspectiveZO(fov, aspect, min, max); 
        }
    };
}