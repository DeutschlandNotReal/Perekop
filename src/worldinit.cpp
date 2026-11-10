#include <PK/Physics/Joint.hpp>
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

    void spawnPlacement(pose at, span<const Model> models) {
        vector<id<BodyHandle>> bids;
        bids.reserve(models.size());
  
        id<BodyHandle> hullid;
        for (const Model& placement : models) {
            auto& Model = ModelHandle::set.create();
            Model.setMesh(placement.getMesh());
            Model.scale = placement.scale;

            auto& Body = BodyHandle::set.create();
            Body.setModel(Model.getID());
            Body.vel = initialVelocity;
            Body.iMass = 1.f / 2.f;

            if (Model.getMesh() == PKG::hull) hullid = Body.getID();

            bids.push(Body.getID());
        }

        BodyHandle hullbody = *hullid;

        for (auto id : bids) {
            if (id == hullid) continue;
            BodyHandle *body = id;
            if (!id) continue;

            vec3 anchor = (hullbody.pose.pos + body->pose.pos) * .5f;
            JointHandle::set.create(Joint::makeWeld())
                .attach0(hullbody, anchor)
                .attach1(*body, anchor)
        }
    }
}

void PKG::onInit::world() {
    spawnPlacement({{4.f, 8.f, 0.f}}, span<const Model>{PKG::placements});
}

void PKG::onStep::world(float dt) {
  for (BodyHandle& body : BodyHandle::set) {
        if (body.tag != 4) continue; 

        vec3 W = getWeight(body.pose.pos);

        float v2 = dot(body.vel, body.vel);

        if (v2 <= 1e-12f || dot(W, W) <= 1e-12f) continue;

        vec3 up = -normalize(W);
        vec3 lift = up - body.vel * (dot(up, body.vel) / v2);
        float lift2 = dot(lift, lift);

        body.impulse(lift / sqrt(lift2) * (sqrt(v2) / body.iMass * liftCoefficient));
    }
}