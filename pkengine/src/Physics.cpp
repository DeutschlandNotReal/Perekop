#define PK_ENGINE_SRC
#include <Internal.hpp>
#include <pk/Body.hpp>
using namespace pk;
using namespace glm;

Body::Body(Model& root): rootid(root.id) {
    
};

void Body::add_pmass(vec3 p, float m) {
    dirty = true;
    const float& x = p.x, y = p.y, z = p.z;
    I += m * mat3(
        y*y+z*z, -x*y, -x*z,
        -x*y, x*x+z*z, -y*z,
        -x*z, -y*z, x*x+y*y
    );
}

void Body::add_mass(float m) { mass += m; }

void Body::add_model(Model& model, float m) {
    // assumes model centre of mass is in the centre of model
    model.body = id;
    add_pmass(pose.lspace_point(model.pose), m);
}

void Body::apply_lforce(vec3 r, vec3 F) {
    force += F;
    torque += cross(r, F);
}

void Body::apply_force(vec3 p, vec3 F) {
    apply_lforce(pose.lspace_point(p), F);
}

mat3 Body::inverse_inertia() {
    if (dirty) invI = inverse(I);
    dirty = false;
    return invI;
}

void Body::intergrate(const float& dt) {
    pose += (vel += force * (dt / mass)) * dt;

    pose.rot = normalize(pose.rot * quat(1, .5f * dt * 
        (avel += dt * inverse_inertia() * torque )
    ));

    force = torque = vec3{0};
}

void Perekop::step_physics(float dt) {
    for (Body& body : World::bodies) body.intergrate(dt);
}