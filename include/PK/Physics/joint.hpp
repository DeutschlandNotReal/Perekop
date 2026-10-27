#pragma once
#include <PK/Math/pose.hpp>

namespace pk::Physics {
    enum class JointType {
        Spring, Solid, Angular
    };

    class Joint {
        public:
            JointType type;
            uint bodyA, bodyB;
            vec3 attA, attB;

            union {
                struct { float stiffness, length; }; // spring
                struct { float maxstrength; };       // solid
                struct { float maxangle; };          // angular
            };

            static Joint SolidType(float maxstrength) noexcept {
                Joint J;
                J.type = JointType::Solid;
                J.maxstrength = maxstrength;
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

            Joint& AttachA(uint BodyID, vec3 pos) noexcept { bodyA = BodyID; attA = pos; return *this; }
            Joint& AttachB(uint BodyID, vec3 pos) noexcept { bodyB = BodyID; attB = pos; return *this; }
    };
}