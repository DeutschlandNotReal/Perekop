#pragma once
#include <PK/Math/pose.hpp>
#include <PK/Render/mesh.hpp>

namespace pk::Physics { 
    mat3 GetUniformInertia(
        span<Mesh::Vertex> vertices
    ) noexcept;

    class Body {
        unsigned id{0};
        public:    
            void AddInertia(
                mat3 inertia,
                vec3 rel
            ) noexcept;

            void Impulse(
                vec3 localforce, 
                vec3 localpoint
            ) noexcept;

            vec3 Velocity() const noexcept;
            quat AngleVel() const noexcept;
            float Mass()    const noexcept;
    };
}