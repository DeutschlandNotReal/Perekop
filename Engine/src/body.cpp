#include <PK/Physics/body.hpp>
#include <PK/Math/clamp.hpp>
using namespace pk;

vec3 pk::inverseInertia(span<const mesh::vertex> vertices) noexcept {
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
    return vec3{1.f} / pk::max(inertia, vec3{1e-6f});
}

void body::impulse(vec3 force) noexcept {
    force += force;
}

void body::impulse(vec3 force, vec3 point) noexcept {
    impulse(force);
    torque += cross(point, force);
}

void body::impulseLocal(vec3 force) noexcept {
    impulse(worldspace(force, pose));
}

void body::impulseLocal(vec3 force, vec3 O) noexcept { 
    impulse(worldspace(force, pose), worldspace(O, pose.rot));
} 