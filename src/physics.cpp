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
inline constexpr float dragK{.01f};

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

vec3 Physics::GetGravity(vec3 o) noexcept {
    return dot(o, o) != 0.f ? normalize(o) * -9.81f : o;
}

float t{0};

void Perekop::PhysicsStep(float dt) noexcept {
    t += dt;

    // collisions
    World::bodySeparator.seperate(World::bodies);
    for (auto &entry : World::bodies) {
        Body &a = entry;
        vec3 ext = abs(World::meshes[a.meshid].ext * a.scale);
        float radius = length(ext);

        for (uint id : World::bodySeparator.near(a.pose.pos, vec3{radius})) {
            Body &b = World::bodies[id];
            if (id <= entry.id || !a.collidable || !b.collidable) continue;
            if (a.collisionId == b.collisionId) continue;
            
            Physics::Collide(a, World::bodies[id], dt);
        }
    }

    Physics::SolveJoints(dt);

    // integrator
    for (Body &body : World::bodies) {
        float inverseMass = body.GetiMass();
        if (inverseMass > 0.f) {
            float iMdt = inverseMass * dt;

            vec3 drag = -dragK * length(body.vel) * body.vel;
            vec3 dVel = (body.F + drag) * iMdt + GetGravity(body.pose.pos) * dt;
            body.vel  += dVel;
            body.pose += body.vel * dt; // + dVel * hdt;

            const auto& mesh = World::meshes[body.meshid];
            vec3 inertia = mesh.invInertia / (body.scale * body.scale);
            mat3 rot = body.pose;

            vec3 dAVel = rot * inertia * (transpose(rot) * body.mom) * dt;
            body.avel += dAVel;

            vec3 avel = body.avel - dAVel * .5f;
            float a2 = dot(avel, avel);
            if (a2 > 0.f) {
                float a = sqrt(a2);
                body.pose.rot = angleAxis(a * dt, avel * (1.f / a)) * body.pose.rot;
            }
        }

        body.pose.rot = normalize(body.pose.rot);
        body.F = body.mom = vec3{0.f};
    }

    Physics::CorrectJoints();
}