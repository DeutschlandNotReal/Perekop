#pragma once
#include <PK/Physics/model.hpp>
#include <PK/Physics/shape.hpp>
#include <PK/Render/mesh.hpp>

namespace pk { 
    namespace Physics {
        vec3 InverseInertia(
            span<const Mesh::Vertex> vertices
        ) noexcept;
    
        vec3 GetGravity(vec3 O) noexcept;
    
        struct ForceContext {
            vec3 F{0.f}, mom{0.f};
        };

        struct MotionContext {
            vec3 vel{0.f}, avel{0.f};
        };
    }

    class Body : public Model, public Physics::ForceContext, public Physics::MotionContext {
        float mass{0}, imass{0};
        static uint NextCollisionId() noexcept {
            static uint nextId{0};
            return ++nextId;
        }

        public:
            float GetMass()  const noexcept { return mass; }
            float GetiMass() const noexcept { return imass; }

            void SetMass(float m) noexcept {
                mass = m;
                imass = m != 0 ? 1.f / m : m;
            }
            
            void SetMassless() noexcept { mass = imass = 0; }

            Physics::ShapeType shapetype{Physics::ShapeType::Box};
            bool collidable{true};
            uint collisionId{NextCollisionId()};
        
            void Impulse(vec3 force) noexcept;
            void Impulse(vec3 force, vec3 point) noexcept;

            void ImpulseLocal(vec3 force) noexcept;
            void ImpulseLocal(vec3 force, vec3 point) noexcept;

            vec3 GetPointVelocity(vec3 p) const noexcept {
                return vel + cross(avel, p - pose.pos);
            }
    };
}