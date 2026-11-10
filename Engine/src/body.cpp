#include <PK/Physics/Body.hpp>
#include <PK/Math/clamp.hpp>
#include <PK/world.hpp>
using namespace pk;

vec3 pk::inverseInertia(span<const Mesh::Vertex> vertices) noexcept {
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

void Body::impulse(vec3 F, vec3 r) noexcept {
    force  += F;
    torque += cross(r, F);
}


// ludicrous way to write R * i * transpose(R)
// but a lil bit faster (opposite values are equal)
mat3 Body::getWorldInertia() const noexcept {
    mat3 R = (mat3) pose;
    vec3 i = getInertia();

    mat3 T{R[0] * i.x, R[1] * i.y, R[2] * i.z};

    float 
        x {T[0].x*R[0].x + T[1].x*R[0].y + T[2].x*R[0].z},
        y {T[0].y*R[1].x + T[1].y*R[1].y + T[2].y*R[1].z},
        z {T[0].z*R[2].x + T[1].z*R[2].y + T[2].z*R[2].z},

        a {T[0].x*R[1].x + T[1].x*R[1].y + T[2].x*R[1].z},
        b {T[0].x*R[2].x + T[1].x*R[2].y + T[2].x*R[2].z},
        c {T[0].y*R[2].x + T[1].y*R[2].y + T[2].y*R[2].z};

    return {
        x, a, b,
        a, y, c,
        b, c, z 
    };
}

void Body::setModel(id<Model> mid) {
    modelid = mid;
    
}