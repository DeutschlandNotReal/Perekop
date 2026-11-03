#define PK_INTERNAL
#include <common.hpp>
#include <PK/Connections/step.hpp>
#include <PK/Render/lighting.hpp>
#include <PK/Render/window.hpp>
#include <PK/Scene/world.hpp>
#include <PK/Systems/gamecamera.hpp>
#include <PK/Util/curve.hpp>

#include <cstdlib>
#include <cmath>
#include <cstdio>

using namespace Perekop; 
using namespace pk;

const std::filesystem::path assets{"assets"};
Material gamemat;

PKG::GameCamera camera;

// -1 to 1
float urand() noexcept { 
    return (float) (rand() % 1000 - 500) / 500.f;
}

// l <= x <= h
template <typename T>
T irand(T l, T h) noexcept {
    return mod((T)urand(), (h - l)) + l;
}

vec3 rnvec() noexcept {
    return normalize(vec3{urand(), urand(), urand()});
}

vec3 rvec(float r) noexcept {
    return rnvec() * pow((urand() + 1) * .5f, 1.f/3.f) * r; 
}

void Perekop::PostRender() {}
void Perekop::PreRender() {}

vector<vector<Model>> modelPlacements;

vector<uint> placementids;
uint prop, rootbody, rootmesh;
constexpr float propellerThrust{10.f};
constexpr float propellerSpin{20.f};

void SpawnPlacement(pose at, const vector<Model>& models) {
    placementids.clear();
    for (const Model& m : models) {
        auto& B = World::bodies.push();
        B.pose = worldspace(m.pose, at);
        B.meshid = m.meshid;
        B.scale = m.scale;
        B.SetMass(irand(1.f, 3.f));
        if (m.meshid == prop)
            B.avel = B.pose.Right() * propellerSpin;
        placementids.emplace(B.id);
    }

    if (placementids.empty()) return;

    for (uint id : placementids)
        World::bodies[id].collisionId = placementids[0];

    uint supportId = placementids[0];
    for (uint id : placementids) {
        if (World::bodies[id].meshid != prop) {
            supportId = id;
            break;
        }
    }

    entry<Body>& support = World::bodies[supportId];
    for (uint id : placementids) {
        if (id == supportId) continue;

        entry<Body>& B = World::bodies[id];
        bool propeller = B.meshid == prop;
        vec3 anchor = propeller ? B.pose.pos
                    : (support.pose.pos + B.pose.pos) * .5f;
        Physics::Joint joint = Physics::Joint::WeldType();
        if (propeller) {
            vec3 axis = B.pose.Right();
            joint = Physics::Joint::HingeType(
                localspace(axis, support.pose.rot),
                localspace(axis, B.pose.rot)
            );
        }

        World::joints.push(joint)
            .AttachA(support, localspace(anchor, support.pose))
            .AttachB(B, localspace(anchor, B.pose));

    }

    entry<Body>& Root = World::bodies[rootbody];
    World::joints.push(Physics::Joint::RopeType(50))
        .AttachA(support, vec3{0.f})
        .AttachB(Root, vec3{0.f});
}

void Perekop::OnLaunch() {
    printf("Game begin\n");
    Window::SetIcon(assets/"images/icon.ico");
    CurrentCamera = &camera;
    camera.pose = pose::LookAt({0, 1000, 55}, {0, 0, 0});
    camera.Init();

    using namespace Lighting;
    using namespace Render;
    Light = pose::LookAt({50, 36, 22}, {0, 2, 0});

    gamemat.shader = {
        {ShaderType::Vertex,   assets/"shaders/vert.glsl"},
        {ShaderType::Fragment, assets/"shaders/frag.glsl"}
    };
        
    gamemat.textures[0] = Lighting::Material = assets/"images/test.jpg";
    
    auto& placement = modelPlacements.emplace();
    MeshImporter::Import(assets/"models/zelensky.glb", [&](MeshImporter::glbValue type, std::string_view name, void* rawdata) {
        switch (type) {
            case MeshImporter::glbValue::Mesh:
                MeshImporter::MeshData& data = *(MeshImporter::MeshData*)rawdata;

                printf(" loading mesh '%*s' (vtx: %i, ind: %i)\n", (int) name.size(), name.cbegin(), data.mesh->vtx.size(), data.mesh->ind.size());
                auto &imesh = World::meshes.push(std::move(*data.mesh));
                imesh.material = &gamemat;
                imesh.Load();

                if (name == "propeller") prop = imesh.id;

                placement.push(Model{.meshid = imesh.id, .pose = data.pose, .scale = vec3{1.f}});
        }
    });

    MeshImporter::Import(assets/"models/arrow.glb", [&](MeshImporter::glbValue type, std::string_view name, void* rawdata) {
        if (type == MeshImporter::glbValue::Mesh) {
            printf(" arrow? %*s\n", (int) name.size(), name.cbegin());
            MeshImporter::MeshData& data = *(MeshImporter::MeshData*)rawdata;
            auto& mesh = World::meshes.push(std::move(*data.mesh));
            mesh.material = &gamemat;
            mesh.Load();
            rootmesh = mesh.id;
        }
    });

    auto& rbody = World::bodies.push();
    rbody.SetMassless();
    rbody.pose = (vec3{0, 1050, 0});
    rbody.meshid = rootmesh;
    rootbody = rbody.id;
    rbody.collisionId = rbody.id;

    for (int i{0}; i < 50; i++) {
        pose at = pose::LookAlong(rvec(25), rvec(1));
        SpawnPlacement(at + vec3{0, 1000, 0}, placement);
    }

    //camera.Target(World::bodies[3]);
}

Curve<float> lift {{
    {.x = 3, .t = 4},
    {.x = 4, .t = 8}
}};

void Perekop::OnStep(float dt) {
    camera.Step(dt);

    for (auto& body : World::bodies) {
        if (body.meshid == prop)
            body.Impulse(body.pose.Forward() * propellerThrust);
    }
}


void Perekop::OnExit() {
    printf("game's gone\n");
} 