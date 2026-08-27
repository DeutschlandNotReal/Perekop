#pragma once
#include <PK/pose.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace pk {
    class Camera {
        public:
            pose pose;
            float min{.1f}, max{200.f}, fov{70};

            mat4 view() const noexcept { return pose.inverse(); }

            glm::mat4 proj(float width, float height) const noexcept {
                return glm::perspective(
                    glm::radians(fov),
                    width / height,
                    min,
                    max
                );
            }
    };
}