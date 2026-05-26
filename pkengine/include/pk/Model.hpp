#pragma once
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <pk/Transform.hpp>

namespace pk {
    struct Model {
        short id;
        glm::vec3 scale{1};
        Transform transform;
        glm::mat3x4 get_scaled() const {
            glm::mat3x4 mat = transform.to_mat3x4();
            *((glm::vec3*) &mat[0]) *= scale.x;
            *((glm::vec3*) &mat[1]) *= scale.y;
            *((glm::vec3*) &mat[2]) *= scale.z;
            return mat;
        }
    };
    
    struct Camera {
        float f{200}, n{.1}, fov{75};
        Transform transform;
        glm::mat4 projection(glm::vec2 screen_size) const;
        glm::mat4 view() const;
    };
}