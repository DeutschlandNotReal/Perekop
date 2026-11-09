#pragma once

#include <PK/math/pose.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace pk {
    class camera {
        friend vec3 worldspace(vec3, const camera&) noexcept;
        friend vec3 localspace(vec3, const camera&) noexcept;

        float tfov, degfov;
        public:
            pose pose;
            float min{.1f}, max{2000.f};
  
            float fov() const noexcept { return degfov; }
            float tanFov() const noexcept { return tfov; }
 
            void fov(float newfov) noexcept {
                tfov = tan(radians((degfov = newfov) * .5f));
            }

            camera() noexcept { fov(70.f); }

            mat4 view() const noexcept;
            mat4 projection() const noexcept;
    };

    vec3 worldspace(vec3, const camera& space) noexcept;
    vec3 localspace(vec3, const camera& space) noexcept;
}