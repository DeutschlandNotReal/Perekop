#pragma once
#include <PK/Physics/model.hpp>
#include <PK/Render/mesh.hpp>

namespace pk::Physics { 
    mat3 GetUniformInertia(
        span<Mesh::Vertex> vertices
    ) noexcept;

    class Body {
        public:
            Model model; // inertia is per-mesh
            float mass;
            pose vel;
            pose acc;

            void Impulse(
                vec3 localforce
            ) noexcept;

            void Impulse(
                vec3 localforce, vec3 localpoint
            ) noexcept;
    };

    inline float gravity = 9.81f;
}