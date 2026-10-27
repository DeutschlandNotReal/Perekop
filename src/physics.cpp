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

float g[]  = {9.81f, 4.01f};
vec3 gp[] = {vec3{0}, vec3{55, 55, 55}};
vec3 Physics::GetGravity(vec3 p) noexcept {
    vec3 net{0};

    for (int i{0}; i < 2; i++) {
        vec3 dsp = gp[i] - p;
        float r2 = dot(dsp, dsp);
        r2 = max(r2, 1.f);
        net += dsp * (g[i] / (sqrt(r2)));
    }
    return net;
}

float t{0};
void Perekop::PhysicsStep(double dt) noexcept {
    float fdt = dt;
    t += fdt;
    g[1] = sin(t * 0.05f) * 4.01f;

    // collisions (n body for now)
    for (uint i{1}; i < World::bodies.size()+1; ++i) {
        Body& a = World::bodies[i];

        for (uint j{i+1}; j < World::bodies.size()+1; ++j) {
            Physics::Collide(a, World::bodies[j], fdt);
        }
    }

    // joints
    for (const Joint& J : World::joints) {
        if (!World::bodies.valid(J.bodyA) || !World::bodies.valid(J.bodyB)) continue;

        Body &A = World::bodies[J.bodyA], &B = World::bodies[J.bodyB];
        switch (J.type) {
            case JointType::Spring: {
                vec3 attA = worldspace(J.attA, A.pose), attB = worldspace(J.attB, B.pose);

                vec3 dsp = attB - attA;
                // F = K(x-L)
                float x = length(dsp);

                vec3 F = dsp * (J.stiffness * (1.f - J.length/x));
                A.Impulse(F, attA);
                B.Impulse(-F, attB);
            };
        }
    }

    
    float loss = 1.f-.5f * fdt;
    // integrator
    for (Physics::Body& body : World::bodies) {
        if (body.meshid == 0) continue;

        float iMdt = body.GetiMass() * fdt;
        body.F += GetGravity(body.pose.pos);
        body.vel += body.F * iMdt;
        //body.vel *= loss;

        mat3 rot = body.pose;
        body.avel += rot * World::meshes[body.meshid]->invInertia * transpose(rot) * body.mom * iMdt;
        
        body.pose.pos += body.vel * fdt;
        body.pose.rot *= quat{body.avel * fdt};
        body.F = body.mom = vec3{0.f};
    }
}