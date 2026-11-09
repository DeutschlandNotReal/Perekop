#define PK_INTERNAL
#include "./internal.hpp"
#define GLM_ENABLE_EXPERIMENTAL

#include <PK/Physics/overlap.hpp>
#include <PK/Physics/plane.hpp>
#include <PK/Physics/ray.hpp>
#include <PK/Physics/body.hpp>
#include <PK/Math/clamp.hpp>
#include <PK/world.hpp>

#include <cmath>
#include <limits>
#include <glm/gtx/component_wise.hpp>

using namespace pk;
using namespace pk::physics;
inline constexpr float inf = std::numeric_limits<float>::infinity();
inline constexpr float dragK{.005f};

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

float ray::intersectCube(vec3 ext, vec3 localpos) const noexcept {
    vec3 p = pos - localpos;
    vec3 inv = 1.f / dir;

    vec3 a = (-ext - p) * inv;
    vec3 b = ( ext - p) * inv;

    float enter = compMax(pk::min(a, b));
    float exit  = compMin(pk::max(a, b));

    return (enter > exit || exit < 0) ? inf : pk::max(enter, 0.f);
}

float ray::intersectBall(float r, vec3 localpos) const noexcept {
    vec3 oc = pos - localpos;

    float b = dot(oc, dir);
    float h = r * r - dot(oc, oc) + b * b;

    if (h < 0) return inf;

    float s = sqrt(h);
    float t = b - s;

    return t >= 0 ? t : b + s;
}

float plane::distance(vec3 point) const noexcept {
    return dot(point - pos, nor);
}

vec3 plane::project(vec3 point) const noexcept {
    return point - distance(point) * nor;
}

vec3 pk::getWeight(vec3 o) noexcept {
    return dot(o, o) != 0.f ? normalize(o) * -9.81f : o;
}

void Perekop::physicsStep(float dt) noexcept {
    physics::solveJoints(dt);

    // integrator
    for (body &body : world::bodies) {
        if (body.iMass > 0.f) {
            float iMdt = body.iMass * dt;

            vec3 drag = -dragK * length(body.vel) * body.vel;
            vec3 dVel = body.force * iMdt + (drag + getWeight(body.pose.pos)) * dt;
            body.vel  += dVel;
            body.pose += body.vel * dt; // + dVel * hdt;

            const auto& mesh = world::meshes[body.meshid];
            vec3 inertia = mesh.invInertia / (body.scale * body.scale);
            mat3 rot = body.pose;

            vec3 dAVel = rot * (inertia * body.iMass) * (transpose(rot) * body.torque) * dt;
            body.avel += dAVel;

            vec3 angularVelocity = body.avel - dAVel * .5f;
            float a2 = dot(angularVelocity, angularVelocity);
            if (a2 > 0.f) {
                float a = sqrt(a2);
                body.pose.rot = angleAxis(a * dt, angularVelocity * (1.f / a)) * body.pose.rot;
            }
        }

        body.pose.rot = normalize(body.pose.rot);
        body.force = body.torque = vec3{0.f};
    }

    physics::correctJoints();
}