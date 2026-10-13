#define PK_INTERNAL
#define GLM_ENABLE_EXPERIMENTAL
#include <common.hpp>

#include <PK/Physics/overlap.hpp>
#include <PK/Physics/plane.hpp>
#include <PK/Physics/ray.hpp>
#include <PK/Physics/body.hpp>
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

vec3 Physics::TestSAT(Shape A, Shape B) noexcept {
    if (A.type != ShapeType::Box || B.type != ShapeType::Box)
        return {0.f, 0.f, 0.f};

    mat3 RA = mat3_cast(A.pose.rot);
    mat3 RB = mat3_cast(B.pose.rot);
    vec3 delta = B.pose.pos - A.pose.pos;

    vec3 best{0.f};
    float bestDist = std::numeric_limits<float>::infinity();
    bool colliding = true;

    auto TestAxis = [&](vec3 axis) noexcept {
        if (!colliding || dot(axis, axis) <= 1e-6f) return;
        axis = normalize(axis);

        float ra = abs(dot(axis, RA[0])) * A.ext.x +
                   abs(dot(axis, RA[1])) * A.ext.y +
                   abs(dot(axis, RA[2])) * A.ext.z;
        float rb = abs(dot(axis, RB[0])) * B.ext.x +
                   abs(dot(axis, RB[1])) * B.ext.y +
                   abs(dot(axis, RB[2])) * B.ext.z;

        float d = abs(dot(axis, delta));
        if (d > ra + rb) {
            colliding = false;
            return;
        }

        float depth = ra + rb - d;
        if (depth < bestDist) {
            bestDist = depth;
            best = axis * depth;
        }
    };

    for (unsigned i = 0; i < 3; ++i) {
        TestAxis(RA[i]);
        TestAxis(RB[i]);

        for (unsigned j = 0; j < 3; ++j)
            TestAxis(cross(RA[i], RB[j]));
    }

    return colliding && bestDist != std::numeric_limits<float>::infinity() ? best : vec3{0.f};
}

mat3 Physics::GetUniformInertia(span<const Mesh::Vertex> vertices) noexcept {
    float xx{0}, yy{0}, zz{0}, xy{0}, yz{0}, zx{0};

    for (const auto& v : vertices) {
        const auto& p = v.pos;

        xx += p.x * p.x;
        yy += p.y * p.y;
        zz += p.z * p.z;
        xy -= p.x * p.y;
        yz -= p.y * p.z;
        zx -= p.z * p.x;
    }

    return mat3{
        yy+zz, xy, zx,
        xy, xx+zz, yz,
        zx, yz, xx+yy
    };
}

void Perekop::CacheMesh(const Mesh& mesh) noexcept {
    for (unsigned i = 0; i < World::meshes.size(); ++i) {
        if (&World::meshes[i] != &mesh)
            continue;

        while (invmatrices.size() <= i)
            invmatrices.emplace(mat3{1.f});
        invmatrices[i] = mesh.vtx.empty()
            ? mat3{1.f}
            : inverse(Physics::GetUniformInertia(mesh.vtx));
        return;
    }
}

void Physics::Body::Impulse(vec3 localforce) noexcept {
    if (GetFlag(StateFlag::anchored) || GetFlag(StateFlag::weightless))
        return;

    force += model.pose.rot * localforce;
}

void Physics::Body::Impulse(vec3 localforce, vec3 localpoint) noexcept {
    if (GetFlag(StateFlag::anchored) || GetFlag(StateFlag::weightless))
        return;
    if (model.meshid >= World::meshes.size())
        return;
    if (Perekop::invmatrices.size() <= model.meshid)
        Perekop::CacheMesh(World::meshes[model.meshid]);

    vec3 force = model.pose.rot * localforce;
    vec3 point = model.pose.rot * localpoint;
    this->force += force;
    moment += cross(point, force);
}

float t = 0.f;
void Perekop::PhysicsStep(double dt) noexcept {
    float fdt = dt;
    float friction = 1.f - expf(-.15f * fdt);
    t += fdt;
    float G = (gravity * 0.6f) * fdt * sin(t * 0.5f) + (gravity * .5f);

    for (Physics::Body& body : World::bodies) {
        if (body.GetFlag(Physics::Body::StateFlag::anchored))
            continue;

        body.vel += (body.force / body.mass) * fdt;
        if (!body.GetFlag(Physics::Body::StateFlag::weightless))
            body.vel.y -= gravity;

        if (body.model.meshid < invmatrices.size())
            body.avel += invmatrices[body.model.meshid] * body.moment * fdt;

        float angspeed = length(body.avel) * fdt;
        if (angspeed > 1e-6f) {
            vec3 axis = normalize(body.avel);
            float s, c;
            sincosf(angspeed * .5f, &s, &c);
            quat delta{c, axis.x * s, axis.y * s, axis.z * s};
            body.model.pose.rot = normalize(delta * body.model.pose.rot);
        }

        body.model.pose.pos += body.vel * fdt;
        body.force = {0.f, 0.f, 0.f};
        body.moment = {0.f, 0.f, 0.f};
    }

    for (unsigned i = 0; i < World::bodies.size(); ++i) {
        for (unsigned j = i + 1; j < World::bodies.size(); ++j) {
            Body& a = World::bodies[i];
            Body& b = World::bodies[j];

            // if (a.model.meshid >= World::meshes.size() || b.model.meshid >= World::meshes.size()) continue;
            if (a.GetFlag(Body::StateFlag::anchored) && b.GetFlag(Body::StateFlag::anchored))
                continue;

            const Mesh& ma = World::meshes[a.model.meshid];
            const Mesh& mb = World::meshes[b.model.meshid];
            if (ma.vtx.empty() || mb.vtx.empty()) continue;

            pose pa = a.model.pose;
            pose pb = b.model.pose;
            pa.pos += pa.rot * (ma.center * a.model.scale);
            pb.pos += pb.rot * (mb.center * b.model.scale);

            Shape sa{pa, ma.ext * abs(a.model.scale)};
            Shape sb{pb, mb.ext * abs(b.model.scale)};

            vec3 delta = pb.pos - pa.pos;
            float ra = length(sa.ext), rb = length(sb.ext);
            if (dot(delta, delta) > (ra + rb) * (ra + rb))
                continue;

            mat3 RA = pa, RB = pb;
            vec3 ea = abs(RA[0]) * sa.ext.x + abs(RA[1]) * sa.ext.y + abs(RA[2]) * sa.ext.z;
            vec3 eb = abs(RB[0]) * sb.ext.x + abs(RB[1]) * sb.ext.y + abs(RB[2]) * sb.ext.z;
            if (any(greaterThan(abs(delta), ea + eb)))
                continue;

            vec3 axis = TestSAT(sa, sb);
            if (dot(axis, axis) <= 1e-6f)
                continue;

            if (dot(axis, pb.pos - pa.pos) < 0.f)
                axis = -axis;

            vec3 normal = normalize(axis);
            vec3 correction = normal * (max(length(axis) - .01f, 0.f) * .8f);
            bool anchoredA = a.GetFlag(Body::StateFlag::anchored);
            bool anchoredB = b.GetFlag(Body::StateFlag::anchored);


            if (anchoredA)
                b.model.pose.pos += correction;
            else if (anchoredB)
                a.model.pose.pos -= correction;
            else {
                correction *= .5f;
                a.model.pose.pos -= correction;
                b.model.pose.pos += correction;
            }

            float closing = dot(b.vel - a.vel, normal);
            if (closing < 0.f) {
                vec3 impulse = normal * closing;
                if (anchoredA)
                    b.vel -= impulse;
                else if (anchoredB)
                    a.vel += impulse;
                else {
                    impulse *= .5f;
                    a.vel += impulse;
                    b.vel -= impulse;
                }
            }

            vec3 relative = b.vel - a.vel;
            vec3 ftan = friction * (relative - normal * dot(relative, normal));


            if (anchoredA)
                b.vel -= ftan;
            else if (anchoredB)
                a.vel += ftan;
            else {
                ftan *= .5f;
                a.vel += ftan;
                b.vel -= ftan;
            }

            float spin = 1.f - friction;
            if (!anchoredA)
                a.avel *= spin;
            if (!anchoredB)
                b.avel *= spin;
        }
    }
}