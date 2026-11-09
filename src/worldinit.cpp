#include <PK/Physics/joint.hpp>
#include <PK/Math/clamp.hpp>
#include <PK/tick.hpp>
#include <PK/world.hpp>

#include "./common.hpp"
#include "./functions.hpp"

using namespace pk;
using namespace physics;

namespace {
    constexpr vec3 initialVelocity{0.f, 0.f, -3.f};
    constexpr float liftCoefficient{.02f};

    void spawnPlacement(pose at, span<const model> models) {
        vector<uint> bodyIds;
        bodyIds.reserve(models.size());

        setID hullbody;
        for (const model& model : models) {
            auto& body = world::bodies.create();
            body.pose = worldspace(model.pose, at);
            body.meshid = model.meshid;
            body.scale = model.scale;
            body.vel = initialVelocity;
            body.iMass = 1.f / 2.f;

            if (body.meshid == PKG::hull) hullbody = body.id;
            bodyIds.push(body.id);
        }

        auto& hullobj = world::bodies[hullbody];

        for (uint id : bodyIds) {
            auto& body = world::bodies[id];
            body.collisionId = hullbody;

            if (id == hullbody) continue;

            vec3 anchor = (hullobj.pose.pos + body.pose.pos) * .5f;
            world::joints.create(joint::makeWeld())
                .attach0(hullobj, anchor)
                .attach1(body, anchor);
        }
    }
}

void PKG::onInit::world() {
    spawnPlacement({{4.f, 8.f, 0.f}}, span<const model>{PKG::placements});
}

void PKG::onStep::world(float dt) {
  for (body& body : world::bodies) {
        if (body.meshid != PKG::hull || body.iMass == 0.f) continue;

        vec3 W = getWeight(body.pose.pos);

        float v2 = dot(body.vel, body.vel);

        if (v2 <= 1e-12f || dot(W, W) <= 1e-12f) continue;

        vec3 up = -normalize(W);
        vec3 lift = up - body.vel * (dot(up, body.vel) / v2);
        float lift2 = dot(lift, lift);

        body.impulse(lift / sqrt(lift2) * (sqrt(v2) / body.iMass * liftCoefficient));
    }
}