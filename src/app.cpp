#include <PK/tick.hpp>
#include <PK/world.hpp>
#include <PK/Render/Resources/material.hpp>
#include <PK/Render/window.hpp>
#include <PK/Math/random.hpp>

#include "./loader.hpp"
#include "./movecam.hpp"
using namespace pk;

PKG::MovableCamera camera;

namespace {
    Material mainMaterial;

    void loadShaders() {
        mainMaterial.shader = {
            {ShaderType::vertex,   world::assets/"shaders/vert.glsl"},
            {ShaderType::fragment, world::assets/"shaders/frag.glsl"}
        };

        const Texture& T = handle<Texture>::create(world::assets/"images/test.jpg");

        mainMaterial.textures[0] = T;
    }

    void loadIcon() {
        Window::main.setIcon(world::assets/"images/icon.png");
    }
}

void Perekop::onInit() {
    loadShaders();
    loadIcon();

    Window::main.camera = &camera;
    camera.connect();

    vector<Model> placements;
    PKG::ImportPlacement(
        world::assets/"models/zelensky.glb",
        placements,
        [](MeshHandle& mesh, auto name){
            pk::success(std::format("Loading mesh {}, ID {}", name, (uint)mesh.getID()));
            mesh.tag = name[0];
            mesh.material = &mainMaterial;
            mesh.load();
            mesh.synctraits();
        }
    );

    auto rd = std::random_device();
    RNG rng(rd());

    int k = 200;
    while (--k > 0) {
        vec3 at = randomvec(rng, vec3{-50}, vec3{50});
        vec3 to = randomvec(rng, vec3{-50}, vec3{50});

        pose pose = pose::lookAlong(at, normalize(to));

        for (const Model& place : placements) {
            ModelHandle& model = ModelHandle::create(place);
            model.pose = worldspace(model.pose, pose);

            BodyHandle& body = BodyHandle::create(model);
            body.iMass = pk::rcp(1);
            body.vel = cross(
                getWeight(body.pose.pos), 
                vec3{0, 1, 0}
            ) * 5.f;
        }
    }
}

void Perekop::onStep(float dt) {
    camera.step(dt);
}

void Perekop::onRender() {}

void Perekop::onExit() {}
