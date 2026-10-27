#pragma once
#include <PK/Physics/body.hpp>
using namespace pk;
using namespace pk::Physics;

vec3 Physics::InverseInertia(span<const Mesh::Vertex> vertices) noexcept {
    vec3 mean{0.f};

    for (const auto& v : vertices) mean += v.pos;

    mean /= (float) vertices.size();

    vec3 sum{1e-6f};

    for (const auto& v : vertices) {
        vec3 p = v.pos - mean;
        sum += p * p;
    }

    return 1.f / vec3{sum.y * sum.z, sum.x * sum.z, sum.x * sum.y};
}

void Body::Impulse(vec3 force) noexcept {
    F += force;
}

void Body::Impulse(vec3 F, vec3 O) noexcept {
    Impulse(F);
    mom += cross(F, O);
}

void Body::ImpulseLocal(vec3 force) noexcept {
    Impulse(worldspace(force, pose));
}

void Body::ImpulseLocal(vec3 force, vec3 O) noexcept {
    Impulse(worldspace(force, pose), worldspace(O, pose));
}