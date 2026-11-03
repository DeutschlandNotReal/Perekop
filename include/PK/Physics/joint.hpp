#pragma once
#include <PK/Physics/body.hpp>
#include <PK/Core/set.hpp>
#include <PK/Math/pose.hpp>

namespace pk::Physics {
    void SolveJoints(float dt) noexcept;
    void CorrectJoints() noexcept;

    enum class JointType {
        Spring, Weld, Hinge, Angular, Rope
    };

    class Joint {
        public:
            JointType type;
            uint bodyA, bodyB;
            vec3 attA, attB;

            union {
                struct { float stiffness, length; }; // spring, rope
                struct { float maxangle; };          // angular
            };

            quat relRot{1.f, 0.f, 0.f, 0.f};
            quat rotA{1.f, 0.f, 0.f, 0.f};
            vec3 hingeAxisA{0.f}, hingeAxisB{0.f};

            static Joint WeldType() noexcept {
                Joint J;
                J.type = JointType::Weld;
                return J;
            }

            static Joint HingeType(vec3 axisA, vec3 axisB) noexcept {
                Joint J;
                J.type = JointType::Hinge;
                J.hingeAxisA = normalize(axisA);
                J.hingeAxisB = normalize(axisB);
                return J;
            }

            static Joint SpringType(float stiffness, float length) noexcept {
                Joint J;
                J.type = JointType::Spring;
                J.stiffness = stiffness;
                J.length = length;
                return J;
            }

            static Joint AngleType(float maxangle) noexcept {
                Joint J;
                J.type = JointType::Angular;
                J.maxangle = maxangle;
                return J;
            }

            static Joint RopeType(float length) noexcept {
                Joint J;
                J.type = JointType::Rope;
                J.length = length;
                return J;
            }

            Joint& AttachA(entry<Body> body, vec3 pos) noexcept {
                bodyA = body.id;
                attA = pos;
                rotA = body.pose.rot;
                return *this;
            }

            Joint& AttachB(entry<Body> body, vec3 pos) noexcept {
                bodyB = body.id;
                attB = pos;
                if (type == JointType::Weld)
                    relRot = conjugate(rotA) * body.pose.rot;
                return *this;
            }
    };
}