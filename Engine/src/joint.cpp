#define PK_INTERNAL
#include "./internal.hpp"

#include <PK/Physics/joint.hpp>
#include <PK/Math/extra.hpp>
#include <PK/world.hpp>

#include <cmath>

using namespace pk;
using namespace physics;

namespace {
    void applyImpulse(Body& Body, vec3 impulse, vec3 r, mat3 I) noexcept {
        Body.vel  += impulse * Body.iMass;
        Body.avel += I * cross(r, impulse);
    }

    mat3 pointMass(const Body& Body, vec3 r, mat3 invInertia) noexcept {
        mat3 result{Body.iMass};
        for (uint i{0}; i < 3; i++) {
            vec3 axis{0.f};
            axis[i] = 1.f;
            result[i] += cross(invInertia * cross(r, axis), r);
        }
        return result;
    }
}

void solveJoints(float dt) noexcept {
    float idt = 1.f / dt;

    for (uint iteration{0}; iteration < 8; iteration++) {
        for (JointHandle& J : JointHandle::items) {
            if (iteration > 0 && J.type == JointType::spring) continue;

            auto &AttA = J.A, &AttB = J.B;

            BodyHandle &A = *AttA.bodyID.get();
            BodyHandle &B = *AttB.bodyID.get();
            
            if (A.iMass * B.iMass == 0.f) continue;

            vec3 posA{worldspace(AttA.pos, A.pose)}, 
                 posB{worldspace(AttB.pos, B.pose)};
                 
            vec3 dsp{posB - posA};

            switch (J.type) {
                case JointType::spring: {
                    float x{length(dsp)};
                    if (x < .01f) continue;

                    vec3 rA = posA - A.pose.pos;
                    vec3 rB = posB - B.pose.pos;
                    vec3 force = dsp * (J.stiffness * (x - J.length) / x);
                    A.impulse(force, rA);
                    B.impulse(-force, rB);
                    break;
                }
                
                case JointType::rope: { 
                    break;
                }
                
                case JointType::weld: { 
                    break;
                }
            }
        }
    }
}

