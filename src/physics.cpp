#define PK_INTERNAL
#define GLM_ENABLE_EXPERIMENTAL
#include <common.hpp>

#include <PK/Physics/overlap.hpp>
#include <PK/Physics/plane.hpp>
#include <PK/Physics/ray.hpp>
#include <PK/Physics/body.hpp>
#include <PK/Physics/collision.hpp>
#include <PK/Scene/world.hpp>

#include <cmath>
#include <limits>
#include <glm/gtx/component_wise.hpp>

using namespace pk;
using namespace pk::Physics;
inline constexpr float inf = std::numeric_limits<float>::infinity();

ray worldspace(ray r, pose p) noexcept {
    return {worldspace(r.pos, p), worldspace(r.dir, p.rot)};
}

ray worldspace(ray r, vec3 p) noexcept {
    return {worldspace(r.pos, p), r.dir};
}

ray localspace(ray r, pose p) noexcept {
    return {localspace(r.pos, p), localspace(r.dir, p.rot)};
}

ray localspace(ray r, vec3 p) noexcept {
    return {localspace(r.pos, p), r.dir};
}

plane worldspace(plane p, pose s) noexcept {
    return {worldspace(p.pos, s), worldspace(p.nor, s.rot)};
}

plane worldspace(plane p, vec3 s) noexcept {
    return {worldspace(p.pos, s), p.nor};
}

plane localspace(plane p, pose s) noexcept {
    return {localspace(p.pos, s), localspace(p.nor, s.rot)};
}

plane localspace(plane p, vec3 s) noexcept {
    return {localspace(p.pos, s), p.nor};
}

float ray::IntersectCube(vec3 ext, vec3 localpos) const noexcept {
    vec3 p = pos - localpos;
    vec3 inv = 1.f / dir;

    vec3 a = (-ext - p) * inv;
    vec3 b = ( ext - p) * inv;

    float enter = compMax(min(a, b));
    float exit  = compMin(max(a, b));

    return (enter > exit || exit < 0) ? inf : max(enter, 0.f);
}

float ray::IntersectBall(float r, vec3 localpos) const noexcept {
    vec3 oc = pos - localpos;

    float b = dot(oc, dir);
    float h = r * r - dot(oc, oc) + b * b;

    if (h < 0) return inf;

    float s = sqrt(h);
    float t = b - s;

    return t >= 0 ? t : b + s;
}

float plane::Distance(vec3 point) const noexcept {
    return dot(point - pos, nor);
}

vec3 plane::Project(vec3 point) const noexcept {
    return point - Distance(point) * nor;
}

vec3 Physics::GetGravity(vec3) noexcept {
    //return vec3{0.f, -9.81f, 0.f};
    return vec3{0.f};
}

float t{0};
void correct(Body& A, Body& B, vec3 dsp, float x) noexcept {
    float invMA{A.GetiMass()}, invMB{B.GetiMass()};
    float L{length(dsp)};

    vec3 k = (L - x) / ((invMA + invMB) * L) * dsp;

    A.pose += k * invMA;
    B.pose -= k * invMB;
}

constexpr float blend{.5f};
void correctvel(Body& A, Body& B, vec3 dsp, float x, float idt) noexcept {
    float invMA{A.GetiMass()}, invMB{B.GetiMass()};
    float L{length(dsp)};

    vec3 k = (L - x) * idt * blend / ((invMA + invMB) * L) * dsp;

    A.vel = (1.f - blend) * A.vel * k * invMA;
    B.vel = (1.f - blend) * B.vel * k * invMB ;
}

void Perekop::PhysicsStep(float dt) noexcept {
    t += dt;
    float idt = 1.f / dt;

    // collisions
    World::bodySeparator.seperate(World::bodies);
    for (auto &entry : World::bodies) {
        Body &a = entry;
        float radius = abs(World::meshes[a.meshid]->ext.x * a.scale.x);

        for (uint id : World::bodySeparator.near(a.pose.pos, vec3{radius})) {
            Body &b = World::bodies[id];
            if (id <= entry.GetId() || !a.colgroup || !b.colgroup) continue;
            auto& colgroup = World::collisiongroups[a.colgroup];

            if (a.colgroup == b.colgroup && !colgroup->selfCollide) continue;
            
            Physics::Collide(a, World::bodies[id], dt);
        }
    }

    // joints
    for (const Joint &J : World::joints) {
        if (!World::bodies.valid(J.bodyA) || !World::bodies.valid(J.bodyB)) continue;

        Body &A = World::bodies[J.bodyA], &B = World::bodies[J.bodyB];

        vec3 attA{worldspace(J.attA, A.pose)}, attB{worldspace(J.attB, B.pose)};
        vec3 dsp{attB-attA};

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
            };

            case JointType::Rope: {
                float x2{dot(dsp, dsp)};
                if (x2 <= J.length * J.length || x2 < (.01f * .01f)) continue;
                float x{sqrt(x2)};

                vec3 rA = attA - A.pose.pos;
                vec3 rB = attB - B.pose.pos;
                vec3 F = dsp * (J.stiffness * (x - J.length) / x);

                A.Impulse(F, rA);
                B.Impulse(-F, rB);

                //correct(A, B, dsp, J.length);
                //correct(A, B, dsp, J.length); 

                break;
            }
        }
    }

    // integrator
    float hdt = dt * .5f;
    for (Physics::Body &body : World::bodies) {
        float inverseMass = body.GetiMass();
        if (inverseMass > 0.f) {
            float iMdt = inverseMass * dt;

            vec3 dVel = body.F * iMdt + GetGravity(body.pose.pos) * dt;
            body.vel  += dVel;
            body.pose += body.vel * dt + dVel * hdt;

            const auto& mesh = *World::meshes[body.meshid];
            vec3 inertia = mesh.invInertia / (body.scale * body.scale);
            mat3 rot = body.pose;

            vec3 dAVel = rot * inertia * (transpose(rot) * body.mom) * dt;
            body.avel += dAVel;

            float a2 = dot(body.avel, body.avel);
            if (a2 > 0.f) {
                float a = sqrt(a2);
                body.pose.rot *= angleAxis(a * dt, body.avel * (1.f / a) + dAVel * hdt);
            }
        }

        body.F = body.mom = vec3{0.f};
    }
}