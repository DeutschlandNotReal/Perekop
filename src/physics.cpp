#define PK_INTERNAL
#define GLM_ENABLE_EXPERIMENTAL
#include <common.hpp>

#include <PK/Physics/overlap.hpp>
#include <PK/Physics/plane.hpp>
#include <PK/Physics/ray.hpp>
#include <PK/Physics/body.hpp>

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

bool Physics::Test(Sphere S0, Sphere S1, vec3 disp) noexcept {
    float r = S0.r + S1.r;
    return dot(disp, disp) <= r * r;
}

bool Physics::Test(Sphere S, Box B, vec3 disp) noexcept {
    vec3 d = max(abs(disp) - B.ext, 0.f);
    return dot(d, d) <=  S.r * S.r;
}

bool Physics::TestAABB(Box B1, Box B2, vec3 disp) noexcept {
    return all(lessThanEqual(abs(disp), B1.ext + B2.ext));
}

bool Physics::TestOOB(Box B1, Box B2, pose rel) noexcept {
    mat3 R = rel;
    mat3 A{abs(R[0]), abs(R[1]), abs(R[2])};
    vec3 t = abs(rel.pos);

    if (any(greaterThan(t, B1.ext + A * B2.ext)))
        return false;

    vec3 rt = abs(transpose(R) * rel.pos);

    if (any(greaterThan(rt, B2.ext + transpose(A) * B1.ext)))
        return false;

    return
        abs(t.y * R[2][0] - t.z * R[1][0]) <=
            B1.ext.y*A[2][0] + B1.ext.z*A[1][0] +
            B2.ext.y*A[0][2] + B2.ext.z*A[0][1] &&

        abs(t.y * R[2][1] - t.z * R[1][1]) <=
            B1.ext.y*A[2][1] + B1.ext.z*A[1][1] +
            B2.ext.x*A[0][2] + B2.ext.z*A[0][0] &&

        abs(t.y * R[2][2] - t.z * R[1][2]) <=
            B1.ext.y*A[2][2] + B1.ext.z*A[1][2] +
            B2.ext.x*A[0][1] + B2.ext.y*A[0][0] &&

        abs(t.z * R[0][0] - t.x * R[2][0]) <=
            B1.ext.z*A[0][0] + B1.ext.x*A[2][0] +
            B2.ext.y*A[1][2] + B2.ext.z*A[1][1] &&

        abs(t.z * R[0][1] - t.x * R[2][1]) <=
            B1.ext.z*A[0][1] + B1.ext.x*A[2][1] +
            B2.ext.x*A[1][2] + B2.ext.z*A[1][0] &&

        abs(t.z * R[0][2] - t.x * R[2][2]) <=
            B1.ext.z*A[0][2] + B1.ext.x*A[2][2] +
            B2.ext.x*A[1][1] + B2.ext.y*A[1][0] &&

        abs(t.x * R[1][0] - t.y * R[0][0]) <=
            B1.ext.x*A[1][0] + B1.ext.y*A[0][0] +
            B2.ext.y*A[2][2] + B2.ext.z*A[2][1] &&

        abs(t.x * R[1][1] - t.y * R[0][1]) <=
            B1.ext.x*A[1][1] + B1.ext.y*A[0][1] +
            B2.ext.x*A[2][2] + B2.ext.z*A[2][0] &&

        abs(t.x * R[1][2] - t.y * R[0][2]) <=
            B1.ext.x*A[1][2] + B1.ext.y*A[0][2] +
            B2.ext.x*A[2][1] + B2.ext.y*A[2][0];
}

mat3 Physics::GetUniformInertia(span<Mesh::Vertex> vertices) noexcept {
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

 