#pragma once
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <pk/Transform.hpp>

namespace pk {
    struct Model {
        short id;
        Pose pose;
    };
    
    struct Camera {
        float min{.1}, max{200}, fov{75};
        Pose pose;
        glm::mat4 projection(glm::vec2 screen_size) const;
        glm::mat4 view() const;
    };
}