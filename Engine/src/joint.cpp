#define PK_INTERNAL
#include "./internal.hpp"

#include <PK/Physics/joint.hpp>
#include <PK/Math/clamp.hpp>
#include <PK/world.hpp>

#include <cmath>

using namespace pk;
using namespace physics;;

namespace {
    mat3 bodyInverseInertia(const body& body) noexcept {
        if (body.iMass <= 0.f) return mat3{0.f};
        const auto& mesh = world::meshes[body.meshid];
        vec3 inertia = mesh.invInertia * body.iMass / (body.scale * body.scale);
        mat3 rot = body.pose;
        mat3 result{0.f};
        result[0][0] = inertia.x;
        result[1][1] = inertia.y;
        result[2][2] = inertia.z;
        return rot * result * transpose(rot);
    }

    mat3 pointMass(const body& body, vec3 r, mat3 invInertia) noexcept {
        mat3 result{body.iMass};
        for (uint i{0}; i < 3; i++) {
            vec3 axis{0.f};
            axis[i] = 1.f;
            result[i] += cross(invInertia * cross(r, axis), r);
        }
        return result;
    }

    void applyImpulse(body& body, vec3 impulse, vec3 r, mat3 invI, float sign) noexcept {
        body.vel  += impulse * (body.iMass * sign);
        body.avel += invI * cross(r, impulse) * sign;
    }

    vec3 solveAngular(mat3 inverseMass, vec3 delta, vec3 freeAxis) noexcept {
        float axis2 = dot(freeAxis, freeAxis);
        if (axis2 <= 1e-8f) return inverse(inverseMass) * delta;

        vec3 axis = freeAxis / sqrt(axis2);
        vec3 ref = abs(axis.x) < .8f ? vec3{1,0,0} : vec3{0,1,0};
        vec3 u = normalize(cross(axis, ref)), v = cross(axis, u);
        vec3 iu = inverseMass * u, iv = inverseMass * v;
        float uu = dot(u, iu), uv = dot(u, iv), vv = dot(v, iv);
        float du = dot(u, delta), dv = dot(v, delta), det = uu * vv - uv * uv;
        return det > 1e-8f ? u * ((du * vv - dv * uv) / det) +
                              v * ((dv * uu - du * uv) / det) : vec3{0.f};
    }

    struct AngularConstraint {
        vec3 error{0.f}, freeAxis{0.f};
    };

    AngularConstraint getAngularConstraint(const joint& joint, const body& A, const body& B) noexcept {
        AngularConstraint constraint;
        if (joint.type == jointType::weld) {
            quat relRot = conjugate(A.pose.rot) * B.pose.rot;
            quat err = normalize(relRot * conjugate(joint.relRot));
            if (err.w < 0.f) err = -err;
            vec3 axis{err.x, err.y, err.z};
            float sinAngle = length(axis);
            if (sinAngle > 0.f) {
                float angle = static_cast<float>(2.f * atan2(sinAngle, err.w) / sinAngle);
                constraint.error = worldspace(vec3{angle} * axis, A.pose.rot);
            }
            return constraint;
        }

        vec3 axisA = worldspace(joint.hingeAxes[0], A.pose.rot);
        vec3 axisB = worldspace(joint.hingeAxes[1], B.pose.rot);
        vec3 axisError = cross(axisA, axisB);
        float sinAngle = length(axisError);
        float cosAngle = clamp(dot(axisA, axisB), -1.f, 1.f);

        if (sinAngle > 1e-6f) {
            float angle = static_cast<float>(atan2(sinAngle, cosAngle) / sinAngle);
            constraint.error = vec3{angle} * axisError;
        } else if (cosAngle < 0.f) {
            vec3 reference = abs(axisA.x) < .8f ? vec3{1.f, 0.f, 0.f} : vec3{0.f, 1.f, 0.f};
            constraint.error = vec3{3.14159265f} * normalize(cross(axisA, reference));
        }
        constraint.freeAxis = axisA;
        return constraint;
    }
}

// Sequential impulses keep spring, rope, weld, and hinge constraints together...
void physics::solveJoints(float dt) noexcept {
    float idt = 1.f / dt;

    for (uint iteration{0}; iteration < 8; iteration++) {
        for (const joint& J : world::joints) {
            //if (!world::bodies.valid(J.bodyA) || !world::bodies.valid(J.bodyB)) continue;

            if (iteration > 0 && J.type == jointType::spring) continue;

            body& A = world::bodies[J.bodyid[0]], &B = world::bodies[J.bodyid[1]];
            vec3 attA{worldspace(J.points[0], A.pose)}, attB{worldspace(J.points[1], B.pose)};
            vec3 dsp{attB - attA};

            switch (J.type) {
                case jointType::spring: {
                    float x{length(dsp)};
                    if (x < .01f) continue;

                    vec3 rA = attA - A.pose.pos;
                    vec3 rB = attB - B.pose.pos;
                    vec3 force = dsp * (J.stiffness * (x - J.length) / x);
                    A.impulse(force, rA);
                    B.impulse(-force, rB);
                    break;
                }

                case jointType::rope: {
                    float x = length(dsp);
                    if (x <= J.length) continue;

                    vec3 n = dsp / x;
                    vec3 rA = attA - A.pose.pos;
                    vec3 rB = attB - B.pose.pos;
                    float invMassSum = A.iMass + B.iMass;
                    if (invMassSum <= 0.f) continue;

                    mat3 invIA = bodyInverseInertia(A), invIB = bodyInverseInertia(B);
                    mat3 invMass = pointMass(A, rA, invIA) + pointMass(B, rB, invIB);
                    float effectiveInvMass = dot(n, invMass * n);
                    if (effectiveInvMass <= 0.f) continue;

                    float separatingSpeed = dot(B.getVelAt(attB) - A.getVelAt(attA), n);
                    float magnitude = pk::max(separatingSpeed / effectiveInvMass, 0.f);
                    vec3 impulse = n * magnitude;

                    applyImpulse(A, impulse, rA, invIA, 1.f);
                    applyImpulse(B, impulse, rB, invIB, -1.f);
                    break;
                }

                case jointType::weld:
                case jointType::hinge: {
                    if (A.iMass + B.iMass <= 0.f) continue;

                    vec3 rA = attA - A.pose.pos;
                    vec3 rB = attB - B.pose.pos;
                    vec3 dlt = B.getVelAt(attB) - A.getVelAt(attA);
                    if (iteration == 0) dlt += dsp * (.2f * idt);

                    mat3 invIA = bodyInverseInertia(A), invIB = bodyInverseInertia(B);
                    mat3 linearMass = pointMass(A, rA, invIA) + pointMass(B, rB, invIB);
                    vec3 impulse = inverse(linearMass) * dlt;
                    applyImpulse(A, impulse, rA, invIA, 1.f);
                    applyImpulse(B, impulse, rB, invIB, -1.f);

                    AngularConstraint constraint = getAngularConstraint(J, A, B);
                    mat3 combinedInvInertia = invIA + invIB;
                    vec3 angularDelta = B.avel - A.avel;
                    if (iteration == 0) angularDelta += vec3{.2f * idt} * constraint.error;
                    vec3 angularImpulse = solveAngular(combinedInvInertia, angularDelta, constraint.freeAxis);
                    A.avel += invIA * angularImpulse;
                    B.avel -= invIB * angularImpulse;
                    break;
                }

                case jointType::angular:
                    break;
            }
        }
    }
}

// Positional correction
void physics::correctJoints() noexcept {
    for (uint iteration{0}; iteration < 8; iteration++) {
        for (const joint& J : world::joints) {
            if ((J.type != jointType::weld && J.type != jointType::hinge)) continue;

            body& A = world::bodies[J.bodyid[0]], &B = world::bodies[J.bodyid[1]];
            float invMassSum = A.iMass + B.iMass;
            if (invMassSum <= 0.f) continue;

            vec3 attA = worldspace(J.points[0], A.pose);
            vec3 attB = worldspace(J.points[1], B.pose);
            vec3 correction = (attB - attA) * (.8f / invMassSum);
            A.pose += correction * A.iMass;
            B.pose -= correction * B.iMass;

            AngularConstraint constraint = getAngularConstraint(J, A, B);
            mat3 invIA = bodyInverseInertia(A), invIB = bodyInverseInertia(B);
            vec3 angularCorrection = solveAngular(invIA + invIB, vec3{.8f} * constraint.error, constraint.freeAxis);
            vec3 rotationA = invIA * angularCorrection;
            vec3 rotationB = invIB * angularCorrection;
            float angleA = length(rotationA), angleB = length(rotationB);
            if (angleA > 0.f)
                A.pose.rot = angleAxis(angleA, rotationA / angleA) * A.pose.rot;
            if (angleB > 0.f)
                B.pose.rot = angleAxis(angleB, -rotationB / angleB) * B.pose.rot;
        }
    }
}
