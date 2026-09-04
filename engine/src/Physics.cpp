#define PK_INTERNAL
#define GLM_ENABLE_EXPERIMENTAL
#include <PKINT/internal.hpp>
#include <PK/pose.hpp>
#include <PK/collision.hpp>
#include <limits>
#include <glm/gtx/component_wise.hpp>

inline constexpr float inf = std::numeric_limits<float>::infinity();
namespace pk {

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

    float enter = compMax(min(a, b));
    float exit  = compMin(max(a, b));

    if (enter > exit || exit < 0) return inf;
    return max(enter, 0.f);
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

namespace overlapTests {
    bool SphereSphere(float r1, float r2, vec3 disp) noexcept {
        float r = r1 + r2;
        return dot(disp, disp) <= r * r;
    }

    bool AABB(vec3 ext1, vec3 ext2, vec3 disp) noexcept {
        return all(lessThanEqual(abs(disp), ext1 + ext2));
    }

    bool OBB(vec3 ext1, vec3 ext2, pose rel) noexcept {
        mat3 R = rel;
        mat3 A{abs(R[0]), abs(R[1]), abs(R[2])};
        vec3 t = abs(rel.pos);

        if (any(greaterThan(t, ext1 + A * ext2)))
            return false;

        vec3 rt = abs(transpose(R) * rel.pos);

        if (any(greaterThan(rt, ext2 + transpose(A) * ext1)))
            return false;

        return
            abs(t.y * R[2][0] - t.z * R[1][0]) <=
                ext1.y*A[2][0] + ext1.z*A[1][0] +
                ext2.y*A[0][2] + ext2.z*A[0][1] &&

            abs(t.y * R[2][1] - t.z * R[1][1]) <=
                ext1.y*A[2][1] + ext1.z*A[1][1] +
                ext2.x*A[0][2] + ext2.z*A[0][0] &&

            abs(t.y * R[2][2] - t.z * R[1][2]) <=
                ext1.y*A[2][2] + ext1.z*A[1][2] +
                ext2.x*A[0][1] + ext2.y*A[0][0] &&

            abs(t.z * R[0][0] - t.x * R[2][0]) <=
                ext1.z*A[0][0] + ext1.x*A[2][0] +
                ext2.y*A[1][2] + ext2.z*A[1][1] &&

            abs(t.z * R[0][1] - t.x * R[2][1]) <=
                ext1.z*A[0][1] + ext1.x*A[2][1] +
                ext2.x*A[1][2] + ext2.z*A[1][0] &&

            abs(t.z * R[0][2] - t.x * R[2][2]) <=
                ext1.z*A[0][2] + ext1.x*A[2][2] +
                ext2.x*A[1][1] + ext2.y*A[1][0] &&

            abs(t.x * R[1][0] - t.y * R[0][0]) <=
                ext1.x*A[1][0] + ext1.y*A[0][0] +
                ext2.y*A[2][2] + ext2.z*A[2][1] &&

            abs(t.x * R[1][1] - t.y * R[0][1]) <=
                ext1.x*A[1][1] + ext1.y*A[0][1] +
                ext2.x*A[2][2] + ext2.z*A[2][0] &&

            abs(t.x * R[1][2] - t.y * R[0][2]) <=
                ext1.x*A[1][2] + ext1.y*A[0][2] +
                ext2.x*A[2][1] + ext2.y*A[2][0];
    }

    bool SphereCube(float r, vec3 ext, vec3 disp) noexcept {
        vec3 d = max(abs(disp) - ext, 0.f);
        return dot(d, d) <= r * r;
    }
}}