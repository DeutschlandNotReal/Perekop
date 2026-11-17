#include "./internal.hpp"
#define GLM_ENABLE_EXPERIMENTAL

#include <PK/Physics/overlap.hpp>
#include <PK/Physics/plane.hpp>
#include <PK/Physics/ray.hpp>
#include <PK/Physics/Body.hpp>
#include <PK/Math/extra.hpp>
#include <PK/world.hpp>

#include <limits>
#include <glm/gtx/component_wise.hpp>

using namespace pk;
using namespace pk::physics;
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

float plane::distance(vec3 point) const noexcept {
    return dot(point - pos, nor);
}

vec3 plane::project(vec3 point) const noexcept {
    return point - distance(point) * nor;
}

vec3 pk::getWeight(vec3 o) noexcept {
    return dot(o, o) != 0.f ? normalize(o) * -9.81f : o;
}

void Perekop::stepPhysics(float dt) {
    // integrator
    float hdt2 = .5f * dt * dt;
    for (BodyHandle& body : BodyHandle::items) {
        if (body.iMass > 0.f) {
            vec3 acc = body.force * body.iMass + getWeight(body.pose.pos);
            body.vel  += acc * dt;
            body.pose += body.vel * dt;

            vec3 dAvel{0.f};
            if (dot(body.torque, body.torque) > 0.f) {
                mat3 R = (mat3) body.pose;
                dAvel = R * (body.getInertia() * (body.torque * R)) * dt;
                body.avel += dAvel;
            }

            vec3 w = body.avel - dAvel * .5f;
            float a2 = dot(w, w);

            if (a2 > 1e-12f) {
                float a = sqrt(a2);
                body.pose.rot = normalize(angleAxis(a * dt, w * (1.f / a)) * body.pose.rot);
            }

            if (ModelHandle* m = body.modelID.get()) m->pose = body.pose;
        }

        body.torque = body.force = vec3{0.f};
    }
}