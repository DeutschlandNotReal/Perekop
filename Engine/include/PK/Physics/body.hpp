#pragma once

#include <PK/Physics/model.hpp>
#include <PK/Physics/shape.hpp>
#include <PK/Render/mesh.hpp>

namespace pk { 
    vec3 inverseInertia(span<const mesh::vertex> vertices) noexcept;

    class body : public model{
        public:
            vec3 vel    {0},
                 avel   {0}, 
                 force  {0}, 
                 torque {0};

            float iMass{0.f};
            
            physics::shapeType shapeType{physics::shapeType::box};
            uint collisionId{0};
        
            void impulse(vec3 force) noexcept;
            void impulse(vec3 force, vec3 point) noexcept;

            void impulseLocal(vec3 force) noexcept;
            void impulseLocal(vec3 force, vec3 point) noexcept;

            vec4 getRAxis() const noexcept {
                float speed = length(avel);
                return {speed > 0.f ? avel / speed : vec3{0.f}, speed};
            }

            vec3 getVelAt(vec3 p) const noexcept {
                return vel + cross(avel, p - pose.pos);
            }
    };

    vec3 getWeight(vec3 O) noexcept;
}