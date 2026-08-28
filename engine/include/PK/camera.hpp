#pragma once
#include <PK/pose.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace pk {
    class Camera {
        friend vec3 worldspace(vec3, const Camera&) noexcept;
        friend vec3 localspace(vec3, const Camera&) noexcept;

        float cotfov, degfov;
        public:
            pose pose;
            float min{.1f}, max{2000.f};

            float fov() const noexcept { return degfov; }

            void fov(float newfov) noexcept { 
                cotfov = 1.f / tan(radians((degfov = newfov) * .5f));
            }

            Camera() noexcept { fov(70.f); }

            mat4 view() const noexcept;
            mat4 proj() const noexcept;
    };

    vec3 worldspace(vec3, const Camera& space) noexcept;
    vec3 localspace(vec3, const Camera& space) noexcept;
}