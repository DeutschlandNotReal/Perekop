#define PK_INTERNAL
#include <algorithm>
#include <common.hpp>
#include <PK/Core/array.hpp>
#include <PK/Physics/collision.hpp>
#include <PK/Physics/overlap.hpp>
#include <PK/Scene/world.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>

using namespace pk;
using namespace pk::Physics;

constexpr float eps  = 1e-9f;
constexpr float inf  = std::numeric_limits<float>::infinity();
constexpr vec3 xaxis{1,0,0}, 
               yaxis{0,1,0},
               zaxis{0,0,1},
               zerov{0,0,0};

mat3 abs(mat3 m) noexcept { return mat3{abs(m[0]), abs(m[1]), abs(m[2])}; }

float len2(vec3 v) noexcept { return dot(v, v); }

OverlapResult Physics::GetOverlap(const Shape &A, const Shape &B) noexcept {    
    if (A.type == ShapeType::Box && B.type == ShapeType::Box) {
        pose disp = localspace(B.pose, A.pose);
        mat3 R = (mat3) disp;
        vec3 l = normalize(R[0]), 
             r = normalize(R[1]), 
             u = normalize(R[2]);

        vec3 axes[15] = {
            xaxis, yaxis, zaxis,
            l, r, u,
            
            // cross products
            {0, -l.z, l.y}, {0, -r.z, r.y}, {0, -u.z, u.y},
            {l.z, 0, -l.x}, {r.z, 0, -r.x}, {u.z, 0, -u.x},
            {-l.y, l.x, 0}, {-r.y, r.x, 0}, {-u.y, u.x, 0}
        };

        float minO{inf}; vec3 minaxis;
        vec3 hA = A.ext * .5f, hB = B.ext * .5f;

        mat3 tR = transpose(R);
        for (uint i{0}; i < 15; i++) {
            vec3 axis{axes[i]};

            // analytical way instead of looping through all eight (8) corners
            float extOA = dot(hA, abs(axis));
            float extOB = dot(hB, abs(tR * axis));

            float dst = abs(dot(disp.pos, axis));
            float overlap = (extOA + extOB) - dst;

            if (overlap < 0.f) return {};

            if (overlap < minO) { minaxis = axis; minO = overlap; }
        }

        return {minaxis, minO};
    } else if (A.type == ShapeType::Ball && B.type == ShapeType::Ball) {
        vec3 disp = A.pose - B.pose;
        float r = A.r + B.r;
        float dst2 = dot(disp, disp);

        if (dst2 > r * r) return {};
        float dst{sqrt(dst2)};

        return {disp/dst, r - dst};
    } else {
        Shape Box{A}, Ball{B}; 
        if (B.type == ShapeType::Box) std::swap(Box, Ball);
        pose rel = localspace(Ball.pose, Box.pose);

        vec3 h = Box.ext * .5f;
        vec3 near = clamp(rel.pos, -h, h);

        if (dot(near, near) < eps) near = vec3{eps};

        vec3 axes[4] = {xaxis, yaxis, zaxis, normalize(near)};

        float minO{inf};
        vec3 minaxis;
      
        for (uint i{0}; i < 4; i++) {
            vec3 axis = axes[i];

            float extBox = dot(h, abs(axis));
            float dst = dot(rel.pos, axis);
            float overlap = (extBox + Ball.r) - abs(dst);

            if (overlap < 0.f) return {};

            if (overlap < minO) { 
                minO = overlap; 
                minaxis = (dst < 0.f) ? -axis : axis;
            }
        }
        return {minaxis, minO};
    }
}

mat3 worldInertia(const mat3& R, vec3 i) noexcept {
    mat3 T{R[0] * i.x, R[1] * i.y, R[2] * i.z};

    // ludicrous way to write R * i * transpose(R)
    // but a lil bit faster (opposite values are equal)

    float 
        x {T[0].x*R[0].x + T[1].x*R[0].y + T[2].x*R[0].z},
        y {T[0].y*R[1].x + T[1].y*R[1].y + T[2].y*R[1].z},
        z {T[0].z*R[2].x + T[1].z*R[2].y + T[2].z*R[2].z},

        a {T[0].x*R[1].x + T[1].x*R[1].y + T[2].x*R[1].z},
        b {T[0].x*R[2].x + T[1].x*R[2].y + T[2].x*R[2].z},
        c {T[0].y*R[2].x + T[1].y*R[2].y + T[2].y*R[2].z};

    return mat3{
        x, a, b,
        a, y, c,
        b, c, z 
    };
}

void Physics::Collide(Body& A, Body& B, float dt) noexcept {
    const Mesh &meshA = World::meshes[A.meshid], 
               &meshB = World::meshes[B.meshid];


    Shape shapeA{A.pose, (meshA.ext.x * A.scale.x)},
          shapeB{B.pose, (meshB.ext.x * B.scale.x)};

    auto result = GetOverlap(shapeA, shapeB);
    if (result.overlap < eps) return;

    float iMassNet = (A.GetiMass() * B.GetiMass());
    if (iMassNet < eps) return;
    iMassNet = 1.f / iMassNet;

    float shareA = A.GetiMass() * iMassNet;
    float shareB = B.GetiMass() * iMassNet;

    vec3 n = normalize(B.pose - A.pose);
    vec3 common = n * (dot(n, A.vel * A.GetMass()) - dot(n, B.vel * B.GetMass()));

    A.Impulse(-common * shareA, A.pose.pos + n * shapeA.r);
    B.Impulse(common * shareB, B.pose.pos - n * shapeB.r);

    A.pose += (vec3)result * shareA;
    B.pose -= (vec3)result * shareB;
}