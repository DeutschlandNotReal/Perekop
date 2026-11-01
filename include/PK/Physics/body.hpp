#pragma once
#include <PK/Physics/model.hpp>
#include <PK/Physics/shape.hpp>
#include <PK/Render/mesh.hpp>

namespace pk::Physics { 
    vec3 InverseInertia(
        span<const Mesh::Vertex> vertices
    ) noexcept;

    class Body : public Model {
        float mass{0}, imass{0};

        public:
            float GetMass()  const noexcept { return mass; }
            float GetiMass() const noexcept { return imass; }

            void SetMass(float m) noexcept {
                mass = m;
                imass = m != 0 ? 1.f / m : m;
            }
            
            void SetMassless() noexcept { mass = imass = 0; }

            ShapeType shapetype{ShapeType::Box};
            vec3 vel{0.f}, avel{0.f}, F{0.f}, mom{0.f};
            uint colgroup{0};
        
            void Impulse(vec3 force) noexcept;
            void Impulse(vec3 force, vec3 point) noexcept;

            void ImpulseLocal(vec3 force) noexcept;
            void ImpulseLocal(vec3 force, vec3 point) noexcept;
    };

    extern vec3 GetGravity(vec3 O) noexcept;
}