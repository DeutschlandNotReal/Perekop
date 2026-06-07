#define PK_ENGINE_SRC
// #define PK_DEBUG "physics.cpp"

#include <Internal.hpp>
#include <PKLib/Body.hpp>
using namespace pk;
using namespace glm;

Body::Body(Model& root): rootid(root.id) {
    
};

glm::mat3 Perekop::to_inertia(vec3 lp) {
    const float &x = lp.x, &y = lp.y, &z = lp.z;
    return {
        y*y+z*z, -x*y, -x*z,
        -x*y, x*x+z*z, -y*z,
        -x*z, -y*z, x*x+y*y
    };
};

void Body::add_mass(vec3 p, float m) {
    dirty = true;
    I += m * Perekop::to_inertia(p);
}

void Body::add_model(Model& model, float m) {
    // assumes model centre of mass is in the centre of model
    model.body = id;
    add_mass(pose.lspace_point(model.pose), m);
}

void Body::impulse_local(vec3 r, vec3 F) {
    force += F;
    torque += cross(r, F);
}

void Body::impulse(vec3 p, vec3 F) {
    impulse_local(pose.lspace_point(p), F);
}

mat3 Body::inverse_inertia() {
    if (dirty) invI = inverse(I);
    dirty = false;
    return invI;
}

void Perekop::step_physics(float dt) {
    float hdt = dt * .5;
    for (Body& body : World::bodies) {
        body.pose += ( body.vel += body.force * ( dt / body.mass ) );

        body.pose.rot = normalize(body.pose.rot * quat(1,  
            hdt * ( body.angvel += dt * body.inverse_inertia() * body.torque )
        ));
    }
}