#include <PK/Physics/Joint.hpp>
#include <PK/Math/extra.hpp>
#include <PK/tick.hpp>
#include <PK/world.hpp>
#include <PK/Math/random.hpp>

#include <PK/Math/format.hpp>

#include "./common.hpp"
#include "./functions.hpp"

using namespace pk;
using namespace physics;

#include <PK/debug.hpp>
namespace {
    constexpr vec3 initialVelocity{0.f, 0.f, -3.f};
    constexpr float liftCoefficient{.02f};

    void place(pose at, span<const Model> models, vec3 vel = vec3{0.f}) {
        vector<BodyID> bids;
        bids.reserve(models.size());
  
        BodyID hullid;
        for (const Model& placement : models) {
            auto& Model = ModelHandle::create();
            Model.meshID = placement.meshID;
            Model.scale = placement.scale;
            Model.bounds = placement.bounds;

            BodyHandle& body = BodyHandle::create();
            body.modelID = Model.getID();
            body.iMass = 1.f/2.f;
            body.pose = worldspace(placement.pose, at);

            if (Model.meshID == PKG::hull) {hullid = body.getID(); body.tag = 4; }

            bids.push(body.getID());
            
            body.vel = vel;
        }

        BodyHandle &hullbody = *hullid.get();

        for (BodyID id : bids) {
            if (id == hullid) continue;
            BodyHandle *body = id.get();
            if (!body) continue;

            vec3 anchor = (hullbody.pose.pos + body->pose.pos) * .5f;

            JointID jid = JointHandle::create(Joint::makeWeld()
                .attach0(hullbody, anchor)
                .attach1(*body, anchor)
            ).getID();
        } 
    }
}

void PKG::onInit::world() {
    std::random_device rd;
    RNG rng{rd()};

    int k = 100;
    while (k-- > 0) {
        vec3 pos   = randomvec(rng, vec3{-500}, vec3{500});
        vec3 look  = randomvec(rng, vec3{-50}, vec3{50});
        
        vec3 vel = 10.f * cross(getWeight(pos), vec3{0, 1, 0});

        place(pose::lookAt(pos, look), PKG::placements, vel);
    }
}

#include <PK/Render/window.hpp>

void PKG::onStep::world(float dt) {
    vec3& campos = Window::main.camera->pose.pos; 
    for (BodyHandle& body : BodyHandle::items) {
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
