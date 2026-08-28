#define PK_INTERNAL
#define GLM_ENABLE_EXPERIMENTAL
#include <PKINT/internal.hpp>
#include <PK/body.hpp>
#include <PK/model.hpp>
#include <PK/collision.hpp>

#include <glm/gtx/component_wise.hpp>
using namespace pk;

// worldspace / localspace impls
namespace pk {
    ray worldspace(ray r, pose space) noexcept {
        return {worldspace(r.pos, space), worldspace(r.delta, space.rot)};
    }

    ray worldspace(ray r, vec3 space) noexcept {
        return {worldspace(r.pos, space), r.delta};
    }

    ray localspace(ray r, pose space) noexcept {
        return {localspace(r.pos, space), localspace(r.delta, space.rot)};
    }

    ray localspace(ray r, vec3 space) noexcept {
        return {localspace(r.pos, space), r.delta};
    }

    plane worldspace(plane p, pose space) noexcept {
        return {worldspace(p.pos, space), worldspace(p.nor, space.rot)};
    }

    plane worldspace(plane p, vec3 space) noexcept {
        return {worldspace(p.pos, space), p.nor};
    }

    plane localspace(plane p, pose space) noexcept {
        return {localspace(p.pos, space), localspace(p.nor, space.rot)};
    }

    plane localspace(plane p, vec3 space) noexcept {
        return {localspace(p.pos, space), p.nor};
    }
}

namespace pk::coltests {
    bool sphere(float r0, float r1, vec3 disp) noexcept {
        return dot(disp, disp) <= ((r0+=r1) *= r0);
    }

    bool aabb(vec3 e1, vec3 e2, vec3 disp) noexcept {
       return all(lessThanEqual(abs(disp), e1 + e2));
    }

    bool obb(vec3 e1, vec3 e2, pose rel) noexcept {
        mat3 R = rel; 
        mat3 A{abs(R[0]), abs(R[1]), abs(R[2])}; 
        vec3 t = abs(rel.pos);

        if (any(greaterThan(t, e1 + A * e2))) return false;
        if (any(greaterThan(abs(transpose(R) * rel.pos), e2 + transpose(A) * e1))) return false;

        return
            abs(t.y * R[2][0] - t.z * R[1][0]) <= e1.y*A[2][0] + e1.z*A[1][0] + e2.y*A[0][2] + e2.z*A[0][1] &&
            abs(t.y * R[2][1] - t.z * R[1][1]) <= e1.y*A[2][1] + e1.z*A[1][1] + e2.x*A[0][2] + e2.z*A[0][0] &&
            abs(t.y * R[2][2] - t.z * R[1][2]) <= e1.y*A[2][2] + e1.z*A[1][2] + e2.x*A[0][1] + e2.y*A[0][0] &&
            abs(t.z * R[0][0] - t.x * R[2][0]) <= e1.z*A[0][0] + e1.x*A[2][0] + e2.y*A[1][2] + e2.z*A[1][1] &&
            abs(t.z * R[0][1] - t.x * R[2][1]) <= e1.z*A[0][1] + e1.x*A[2][1] + e2.x*A[1][2] + e2.z*A[1][0] &&
            abs(t.z * R[0][2] - t.x * R[2][2]) <= e1.z*A[0][2] + e1.x*A[2][2] + e2.x*A[1][1] + e2.y*A[1][0] &&
            abs(t.x * R[1][0] - t.y * R[0][0]) <= e1.x*A[1][0] + e1.y*A[0][0] + e2.y*A[2][2] + e2.z*A[2][1] &&
            abs(t.x * R[1][1] - t.y * R[0][1]) <= e1.x*A[1][1] + e1.y*A[0][1] + e2.x*A[2][2] + e2.z*A[2][0] &&
            abs(t.x * R[1][2] - t.y * R[0][2]) <= e1.x*A[1][2] + e1.y*A[0][2] + e2.x*A[2][1] + e2.y*A[2][0];
    }
    
    bool obb_aabb(vec3 obb_e, vec3 aabb_e, pose rel) noexcept { return obb(obb_e, aabb_e, rel); }

    bool box_sphere(vec3 ext, float r, vec3 disp) noexcept {
        vec3 d = max(abs(disp) - ext, 0.f);
        return dot(d, d) <= r * r;
    }

    bool ray_box(ray r, vec3 ext) noexcept {
        vec3 inv = 1.0f / r.delta;
        vec3 t0 = (-ext - r.pos) * inv;
        vec3 t1 = ( ext - r.pos) * inv;

        vec3 lo = min(t0, t1);
        vec3 hi = max(t0, t1);

        float enter = max(max(lo.x, lo.y), lo.z);
        float exit  = min(min(hi.x, hi.y), hi.z);

        return exit >= max(enter, 0.0f) && enter <= 1.0f;
    }

    bool ray_sphere(ray r, float radius) noexcept {
        float t = -dot(r.pos, r.delta) / dot(r.delta, r.delta);
        t = clamp(t, 0.0f, 1.0f);

        vec3 d = r.pos + t * r.delta;
        return dot(d, d) <= radius * radius;
    }

    bool obb_shortcut(vec3 e1, vec3 e2, pose rel) noexcept {
        return sphere(compMax(e1), compMax(e2), rel.pos) && aabb(e1, e2, rel.pos) && obb(e1, e2, rel);
    }
}

// collision
namespace pk {
    bool intersects(ray r, shape s) noexcept {
        switch (s.type) {
            case shape::Cuboid: return coltests::ray_box(r, s.extent);
            case shape::Sphere: return coltests::ray_sphere(r, s.r);
        }
    }

    bool intersects(shape a, shape b, vec3 rel) noexcept {
        switch (a.type) {
            case shape::Cuboid: switch (b.type) {
                case shape::Cuboid: return coltests::aabb(a.extent, b.extent, rel);
                case shape::Sphere: return coltests::box_sphere(a.extent, b.r, rel);
            }
            case shape::Sphere: switch(b.type) {
                case shape::Cuboid: return coltests::box_sphere(b.extent, a.r, rel);
                case shape::Sphere: return coltests::sphere(a.r, b.r, rel);
            }
        }
    }

    bool intersects(shape a, shape b, pose rel) noexcept {
        switch (a.type) {
            case shape::Cuboid: switch (b.type) {
                case shape::Cuboid: return coltests::obb_shortcut(a.extent, b.extent, rel);
                case shape::Sphere: return coltests::box_sphere(a.extent, b.r, rel.pos);
            }
            case shape::Sphere: switch (b.type) {
                case shape::Cuboid: return coltests::box_sphere(b.extent, a.r, rel.pos);
                case shape::Sphere: return coltests::sphere(a.r, b.r, rel.pos);
            }
        }
    }

    float plane::distance(vec3 point) const noexcept {
        return dot(point - pos, nor);
    }

    vec3 plane::project(vec3 point) const noexcept {
        return point - distance(point) * nor;
    }
}

void Perekop::step_physics(float dt) {

}

