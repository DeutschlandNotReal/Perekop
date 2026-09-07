#pragma once
#include <PK/pose.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace pk {
    class Camera {
        friend vec3 worldspace(vec3, const Camera&) noexcept;
        friend vec3 localspace(vec3, const Camera&) noexcept;

        float tfov, degfov;
        public:
            pose pose;
            float min{.1f}, max{2000.f};
  
            float Fov() const noexcept { return degfov; }
            float TanFov() const noexcept { return tfov; }
 
            void Fov(float newfov) noexcept {
                tfov = tan(radians((degfov = newfov) * .5f));
            }

            Camera() noexcept { Fov(70.f); }

            mat4 View() const noexcept;
            mat4 Projection() const noexcept;
    };

    vec3 worldspace(vec3, const Camera& space) noexcept;
    vec3 localspace(vec3, const Camera& space) noexcept;
}