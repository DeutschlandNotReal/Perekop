#pragma once
#include "glm/glm.hpp"

namespace pk {
    struct Camera {
        float f = 100, n = .01f;
        float fov = 75;
        glm::vec3 origin;
        glm::vec3 look;

        // defined in mesh.cpp, dont wanna make a whole new file just for two functions
        inline glm::mat4 view_matrix() const;
        inline glm::mat4 proj_matrix(glm::vec2 dim) const;
    };
}