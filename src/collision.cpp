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
        vec3 hA = A.ext, hB = B.ext;

        mat3 tR = transpose(R);
        for (uint i{0}; i < 15; i++) {
            vec3 axis{axes[i]};
            float axisLength2 = dot(axis, axis);
            if (axisLength2 < eps) continue;
            axis *= 1.f / sqrt(axisLength2);

            // analytical way instead of looping through all eight (8) corners
            float extOA = dot(hA, abs(axis));
            float extOB = dot(hB, abs(tR * axis));

            float dst = abs(dot(disp.pos, axis));
            float overlap = (extOA + extOB) - dst;

            if (overlap < 0.f) return {};

            if (overlap < minO) { minaxis = axis; minO = overlap; }
        }

        return {worldspace(minaxis, A.pose.rot), minO};
    } else if (A.type == ShapeType::Ball && B.type == ShapeType::Ball) {
        vec3 disp = (A.pose - B.pose).pos;
        float r = A.r + B.r;
        float dst2 = dot(disp, disp);

        if (dst2 > r * r) return {};
        float dst = sqrt(dst2);
        vec3 axis = dst2 > eps ? disp / dst : xaxis;

        return {axis, r - dst};
    } else {
        Shape Box{A}, Ball{B}; 
        if (B.type == ShapeType::Box) std::swap(Box, Ball);
        pose rel = localspace(Ball.pose, Box.pose);

        vec3 h = Box.ext;
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
        return {worldspace(minaxis, Box.pose.rot), minO};
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

Shape GetShape(const Body &A, const Mesh &M) noexcept {
    switch (A.shapetype) {
        case Physics::ShapeType::Box:
            return {A.pose, abs(M.ext * A.scale)};
        case Physics::ShapeType::Ball:
            return {A.pose, compMin(abs(M.ext * A.scale))};
    }
    return {A.pose, vec3{0.f}};
}

void Physics::Collide(Body &A, Body &B, float) noexcept {
    const Mesh &meshA = World::meshes[A.meshid], 
               &meshB = World::meshes[B.meshid];

    Shape shapeA{GetShape(A, meshA)}, shapeB{GetShape(B, meshB)};

    auto result = GetOverlap(shapeA, shapeB);
    if (result.overlap < eps) return;

    float invMassA = A.GetiMass(), invMassB = B.GetiMass();
    float invMassSum = invMassA + invMassB;
    if (invMassSum <= 0.f) return;

    vec3 centerDelta = B.pose.pos - A.pose.pos;
    vec3 n = len2(result.axis) > eps ? normalize(result.axis) : xaxis;
    if (dot(centerDelta, n) < 0.f) n = -n;

    float penetration = max(result.overlap - .001f, 0.f);
    vec3 correction = n * (.8f * penetration / invMassSum);
    A.pose -= correction * invMassA;
    B.pose += correction * invMassB;

    float relativeSpeed = dot(B.vel - A.vel, n);
    if (relativeSpeed < 0.f) {
        float impulse = -relativeSpeed / invMassSum;
        A.vel -= n * (impulse * invMassA);
        B.vel += n * (impulse * invMassB);
    }
}

void SpatialSeperator::seperate(const set<Body>& values) {
    entries.clear();
    positions.clear();
    positions.reserve(values.size());

    for (uint i{0}; i < values.size(); i++) {
        const auto& val = values.dataindex(i);
        const Body& body = val;
        vec3 ext = abs(World::meshes[body.meshid].ext * body.scale);
        float radius = length(ext);
        positions.emplace(body.pose.pos, vec3{radius}, val.id);
    }

    auto build = [&](auto&& build, uint first, uint last) -> uint {
        vec3 minext{inf}, maxext{-inf};
        for (uint i{first}; i < last; i++) {
            minext = min(minext, positions[i].centre - positions[i].ext);
            maxext = max(maxext, positions[i].centre + positions[i].ext);
        }

        vec3 centre = (minext + maxext) * .5f;
        vec3 ext = (maxext - minext) * .5f;
        uint index = entries.size();
        entries.emplace(centre, ext);

        if (last - first == 1) {
            entries[index].itemid = positions[first].id;
            return index;
        }

        uint axis = ext.x > ext.y ? (ext.x > ext.z ? 0 : 2) : (ext.y > ext.z ? 1 : 2);
        uint middle = first + (last - first) / 2;
        std::nth_element(
            positions.begin() + first,
            positions.begin() + middle,
            positions.begin() + last,
            [axis](const Position& a, const Position& b) { return a.centre[axis] < b.centre[axis]; }
        );

        entries[index].lhsid = build(build, first, middle);
        entries[index].rhsid = build(build, middle, last);
        return index;
    };

    if (!positions.empty()) build(build, 0, positions.size());
}

span<uint> SpatialSeperator::near(vec3 O, vec3 ext) noexcept {
    nearby.clear();
    ext = abs(ext);

    auto search = [&](auto&& search, uint index){
        const Entry& entry = entries[index];
        vec3 delta = abs(entry.centre - O);
        if (delta.x > entry.ext.x + ext.x ||
            delta.y > entry.ext.y + ext.y ||
            delta.z > entry.ext.z + ext.z) return;

        if (entry.itemid) {
            nearby.emplace(entry.itemid);
            return;
        }

        search(search, entry.lhsid);
        search(search, entry.rhsid);
    };

    if (!entries.empty()) search(search, 0);

    return {nearby.begin(), nearby.end()};
}