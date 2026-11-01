#pragma once
#include <PK/Physics/body.hpp>
using namespace pk;
using namespace pk::Physics;

vec3 Physics::InverseInertia(span<const Mesh::Vertex> vertices) noexcept {
    if (vertices.size() == 0) return vec3{0.f};

    vec3 mean{0.f};

    for (const auto& v : vertices) mean += v.pos;

    mean /= (float) vertices.size();

    vec3 sum{0.f};

    for (const auto& v : vertices) {
        vec3 p = v.pos - mean;
        sum += p * p;
    }

    sum /= (float) vertices.size();
    vec3 inertia = vec3{sum.y + sum.z, sum.x + sum.z, sum.x + sum.y};
    return vec3{1.f} / max(inertia, vec3{1e-6f});
}

void Body::Impulse(vec3 force) noexcept {
    F += force;
}

void Body::Impulse(vec3 force, vec3 point) noexcept {
    Impulse(force);
    mom += cross(point, force);
}

void Body::ImpulseLocal(vec3 force) noexcept {
    Impulse(worldspace(force, pose));
}

void Body::ImpulseLocal(vec3 force, vec3 O) noexcept {
    Impulse(worldspace(force, pose), worldspace(O, pose) - pose.pos);
}