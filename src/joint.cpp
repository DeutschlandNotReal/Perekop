#define PK_INTERNAL
#include <common.hpp>

#include <PK/Physics/joint.hpp>
#include <PK/Scene/world.hpp>

#include <cmath>

using namespace pk;
using namespace pk::Physics;

namespace {
    mat3 inverseInertia(const Body& body) noexcept {
        if (body.GetiMass() <= 0.f) return mat3{0.f};
        const auto& mesh = World::meshes[body.meshid];
        vec3 inertia = mesh.invInertia / (body.scale * body.scale);
        mat3 rot = body.pose;
        mat3 result{0.f};
        result[0][0] = inertia.x;
        result[1][1] = inertia.y;
        result[2][2] = inertia.z;
        return rot * result * transpose(rot);
    }

    mat3 pointMass(const Body& body, vec3 r) noexcept {
        mat3 result{body.GetiMass()};
        mat3 invInertia = inverseInertia(body);
        for (uint i{0}; i < 3; i++) {
            vec3 axis{0.f};
            axis[i] = 1.f;
            result[i] += cross(invInertia * cross(r, axis), r);
        }

        return result;
    }

    vec3 solveAngular(mat3 inverseMass, vec3 delta, vec3 freeAxis) noexcept {
        float axisLength2 = dot(freeAxis, freeAxis);
        if (axisLength2 <= 1e-8f)
            return inverse(inverseMass) * delta;

        vec3 axis = freeAxis / sqrt(axisLength2);
        vec3 reference = abs(axis.x) < .8f ? vec3{1.f, 0.f, 0.f} : vec3{0.f, 1.f, 0.f};
        vec3 u = normalize(cross(axis, reference));
        vec3 v = cross(axis, u);

        float uu = dot(u, inverseMass * u);
        float uv = dot(u, inverseMass * v);
        float vv = dot(v, inverseMass * v);
        float du = dot(u, delta);
        float dv = dot(v, delta);
        float determinant = uu * vv - uv * uv;
        if (determinant <= 1e-8f) return vec3{0.f};

        float impulseU = (du * vv - dv * uv) / determinant;
        float impulseV = (dv * uu - du * uv) / determinant;
        return u * impulseU + v * impulseV;
    }
}

void Physics::SolveJoints(float dt) noexcept {
    float idt = 1.f / dt;

    for (uint iteration{0}; iteration < 8; iteration++) {
        for (const Joint& J : World::joints) {
            if (!World::bodies.valid(J.bodyA) || !World::bodies.valid(J.bodyB)) continue;
            if (iteration > 0 && J.type == JointType::Spring) continue;

            Body& A = World::bodies[J.bodyA], &B = World::bodies[J.bodyB];
            vec3 attA{worldspace(J.attA, A.pose)}, attB{worldspace(J.attB, B.pose)};
            vec3 dsp{attB - attA};

            switch (J.type) {
                case JointType::Spring: {
                    float x{length(dsp)};
                    if (x < .01f) continue;

                    vec3 rA = attA - A.pose.pos;
                    vec3 rB = attB - B.pose.pos;
                    vec3 F = dsp * (J.stiffness * (x - J.length) / x);
                    A.Impulse(F, rA);
                    B.Impulse(-F, rB);
                    break;
                }

                case JointType::Rope: {
                    float x = length(dsp);
                    if (x < .01f) continue;

                    vec3 n = dsp / x;
                    vec3 rA = attA - A.pose.pos;
                    vec3 rB = attB - B.pose.pos;
                    float invMassA = A.GetiMass();
                    float invMassB = B.GetiMass();
                    float invMassSum = invMassA + invMassB;
                    if (invMassSum <= 0.f) continue;

                    if (iteration == 0) {
                        vec3 correction = n * (.2f * (x - J.length) / invMassSum);
                        A.pose += correction * invMassA;
                        B.pose -= correction * invMassB;
                    }

                    mat3 invMass = pointMass(A, rA) + pointMass(B, rB);
                    float effectiveInvMass = dot(n, invMass * n);
                    if (effectiveInvMass <= 0.f) continue;

                    float separatingSpeed = dot(B.GetPointVelocity(attB) - A.GetPointVelocity(attA), n);
                    float targetSpeed = x > J.length
                        ? -.2f * (x - J.length) * idt
                        : (J.length - x) * idt;
                    float magnitude = max((separatingSpeed - targetSpeed) / effectiveInvMass, 0.f);
                    vec3 impulse = n * magnitude;

                    A.vel += impulse * invMassA;
                    B.vel -= impulse * invMassB;
                    A.avel += inverseInertia(A) * cross(rA, impulse);
                    B.avel -= inverseInertia(B) * cross(rB, impulse);
                    break;
                }

                case JointType::Weld:
                case JointType::Hinge: {
                    if (A.GetiMass() + B.GetiMass() <= 0.f) continue;

                    vec3 rA = attA - A.pose.pos;
                    vec3 rB = attB - B.pose.pos;
                    vec3 dlt = B.GetPointVelocity(attB) - A.GetPointVelocity(attA);
                    if (iteration == 0) dlt += dsp * (.2f * idt);

                    mat3 linearMass = pointMass(A, rA) + pointMass(B, rB);
                    vec3 impulse = inverse(linearMass) * dlt;
                    A.vel += impulse * A.GetiMass();
                    B.vel -= impulse * B.GetiMass();
                    A.avel += inverseInertia(A) * cross(rA, impulse);
                    B.avel -= inverseInertia(B) * cross(rB, impulse);

                    vec3 angularError{0.f}, constrainedAxis{0.f};
                    if (J.type == JointType::Weld) {
                        quat relRot = conjugate(A.pose.rot) * B.pose.rot;
                        quat err = normalize(relRot * conjugate(J.relRot));
                        if (err.w < 0.f) err = -err;
                        vec3 axis{err.x, err.y, err.z};
                        float sinAngle = length(axis);
                        if (sinAngle > 0.f)
                            angularError = worldspace(vec3{static_cast<float>(2.f * atan2(sinAngle, err.w) / sinAngle)} * axis, A.pose.rot);
                    } else {
                        vec3 axisA = worldspace(J.hingeAxisA, A.pose.rot);
                        vec3 axisB = worldspace(J.hingeAxisB, B.pose.rot);
                        vec3 axisError = cross(axisA, axisB);
                        float sinAngle = length(axisError);
                        float cosAngle = clamp(dot(axisA, axisB), -1.f, 1.f);
                        if (sinAngle > 1e-6f)
                            angularError = vec3{static_cast<float>(atan2(sinAngle, cosAngle) / sinAngle)} * axisError;
                        else if (cosAngle < 0.f) {
                            vec3 reference = abs(axisA.x) < .8f ? vec3{1.f, 0.f, 0.f} : vec3{0.f, 1.f, 0.f};
                            angularError = vec3{3.14159265f} * normalize(cross(axisA, reference));
                        }
                        constrainedAxis = axisA;
                    }

                    mat3 combinedInvInertia = inverseInertia(A) + inverseInertia(B);
                    vec3 angularDelta = B.avel - A.avel;
                    if (iteration == 0) angularDelta += vec3{.2f * idt} * angularError;
                    vec3 angularImpulse = solveAngular(combinedInvInertia, angularDelta, constrainedAxis);
                    A.avel += inverseInertia(A) * angularImpulse;
                    B.avel -= inverseInertia(B) * angularImpulse;
                    break;
                }

                case JointType::Angular:
                    break;
            }
        }
    }
}

void Physics::CorrectJoints() noexcept {
    for (uint iteration{0}; iteration < 8; iteration++) {
        for (const Joint& J : World::joints) {
            if ((J.type != JointType::Weld && J.type != JointType::Hinge) ||
                !World::bodies.valid(J.bodyA) || !World::bodies.valid(J.bodyB)) continue;

            Body& A = World::bodies[J.bodyA], &B = World::bodies[J.bodyB];
            float invMassA = A.GetiMass(), invMassB = B.GetiMass();
            float invMassSum = invMassA + invMassB;
            if (invMassSum <= 0.f) continue;

            vec3 attA = worldspace(J.attA, A.pose);
            vec3 attB = worldspace(J.attB, B.pose);
            vec3 correction = (attB - attA) * (.8f / invMassSum);
            A.pose += correction * invMassA;
            B.pose -= correction * invMassB;

            vec3 angularError{0.f}, constrainedAxis{0.f};
            if (J.type == JointType::Weld) {
                quat relRot = conjugate(A.pose.rot) * B.pose.rot;
                quat err = normalize(relRot * conjugate(J.relRot));
                if (err.w < 0.f) err = -err;
                vec3 axis{err.x, err.y, err.z};
                float sinAngle = length(axis);
                if (sinAngle > 0.f)
                    angularError = worldspace(vec3{static_cast<float>(2.f * atan2(sinAngle, err.w) / sinAngle)} * axis, A.pose.rot);
            } else {
                vec3 axisA = worldspace(J.hingeAxisA, A.pose.rot);
                vec3 axisB = worldspace(J.hingeAxisB, B.pose.rot);
                vec3 axisError = cross(axisA, axisB);
                float sinAngle = length(axisError);
                float cosAngle = clamp(dot(axisA, axisB), -1.f, 1.f);
                if (sinAngle > 1e-6f)
                    angularError = vec3{static_cast<float>(atan2(sinAngle, cosAngle) / sinAngle)} * axisError;
                else if (cosAngle < 0.f) {
                    vec3 reference = abs(axisA.x) < .8f ? vec3{1.f, 0.f, 0.f} : vec3{0.f, 1.f, 0.f};
                    angularError = vec3{3.14159265f} * normalize(cross(axisA, reference));
                }
                constrainedAxis = axisA;
            }

            mat3 invIA = inverseInertia(A), invIB = inverseInertia(B);
            vec3 angularCorrection = solveAngular(invIA + invIB, vec3{.8f} * angularError, constrainedAxis);
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
