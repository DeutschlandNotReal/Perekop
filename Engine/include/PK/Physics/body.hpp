#pragma once

#include <PK/Physics/Model.hpp>
#include <PK/Physics/shape.hpp>
#include <PK/Render/Mesh.hpp>
#include <PK/Core/set.hpp>

namespace pk { 
    vec3 inverseInertia(span<const Mesh::Vertex> vertices) noexcept;

    class Body {
        public:
            Body() noexcept = default;
            Body(const ModelHandle&) noexcept;

            ModelID modelID;
            vec3 inertia{0.f};

            pose pose;
            vec3 vel    {0},
                 avel   {0}, 
                 force  {0}, 
                 torque {0};

            float iMass{0.f};
            int tag{0};

            physics::shapeType shapeType{physics::shapeType::box};
            void impulse(vec3 force, vec3 point = vec3{0.f}) noexcept;

            vec4 getRAxis() const noexcept {
                float speed = length(avel);
                return {speed > 0.f ? avel / speed : vec3{0.f}, speed};
            }

            vec3 getVelAt(vec3 p) const noexcept {
                return vel + cross(avel, p - pose.pos);
            }

            vec3 getInertia() const noexcept { return inertia * iMass; }
            mat3 getWorldInertia() const noexcept;
    };

    using BodyHandle = handle<Body>;
    using BodyID     = SID<Body>;

    vec3 getWeight(vec3 O) noexcept;
}