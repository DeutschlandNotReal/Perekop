#pragma once

#include <PK/Physics/Body.hpp>
#include <PK/Core/set.hpp>
#include <PK/Math/pose.hpp>

namespace pk {
    void solveJoints(float dt) noexcept;
    void correctJoints() noexcept;

    enum class jointType {
        spring, weld, rope
    };

    class Joint {
        public:
            struct attachment {
                id<BodyHandle> bodyid; 
                vec3 pos{0.f};
            };

            jointType type;

            attachment A, B;

            union {
                struct { float stiffness, length; }; // spring, rope
                //struct { float maxangle; };          // angular
            };

            quat rel{1, 0, 0, 0}, rot0{1, 0, 0, 0};

            quat relRot{1.f, 0.f, 0.f, 0.f};
            quat rotA{1.f, 0.f, 0.f, 0.f};

            vec3 hingeAxes[2] = {vec3{0}, vec3{0}};

            static Joint makeWeld() noexcept {
                return {
                    .type = jointType::weld
                };
            }

            static Joint makeSpring(float stiffness, float length) noexcept {
                return {
                    .type = jointType::spring,
                    .stiffness = stiffness,
                    .length = length
                };
            }

            static Joint makeRope(float length) noexcept {
                return {
                    .type = jointType::rope,
                    .length = length
                };
            }

            Joint& attach0(BodyHandle &Body, vec3 pos) noexcept {
                A = {.bodyid = Body.getID(), .pos = pos};
                
                rotA = Body.pose.rot;
                return *this;
            }

            Joint& attach1(BodyHandle &Body, vec3 pos) noexcept {
                B = {.bodyid = Body.getID(), .pos = pos};
        
                if (type == jointType::weld)
                    relRot = conjugate(rotA) * Body.pose.rot;
                
                return *this;
            }
    };

    using JointHandle = handle<Joint>;
}