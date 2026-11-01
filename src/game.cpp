#define PK_INTERNAL
#include <common.hpp>
#include <PK/Connections/step.hpp>
#include <PK/Render/lighting.hpp>
#include <PK/Render/window.hpp>
#include <PK/Scene/world.hpp>
#include <PK/Systems/camera.hpp>

#include <cstdlib>
#include <cmath>
#include <cstdio>

using namespace Perekop; 
using namespace pk;

const std::filesystem::path assets{"assets"};
Material gamemat;

// -1 to 1
float urand() noexcept { 
    return (float) (rand() % 1000 - 500) / 500.f;
}

// l <= x <= h
int irand(int l, int h) noexcept {
    return rand() % (h - l) + l;
}

vec3 rnvec() noexcept {
    return normalize(vec3{urand(), urand(), urand()});
}

vec3 rvec(float r) noexcept {
    return rnvec() * pow((urand() + 1) * .5f, 1.f/3.f) * r; 
}

void Perekop::OnStep(double dt) {
    PKG::StepCamera(pk::Lighting::Camera, dt);
}

void Perekop::OnRender() {

}

void Perekop::OnLaunch() {
    printf("Game begin\n");
    Window::SetIcon(assets/"images/icon.ico");

    using namespace Lighting;
    using namespace Render;
    Light = pose::LookAt({50, 36, 22}, {0, 2, 0});
    Lighting::Camera.pose = pose::LookAt({0, 0, 55}, {0, 0, 0});
    PKG::InitCamera(Lighting::Camera);

    gamemat.shader = {
        {ShaderType::Vertex,   assets/"shaders/vert.glsl"},
        {ShaderType::Fragment, assets/"shaders/frag.glsl"}
    };
        
    gamemat.textures[0] = Lighting::Material = assets/"images/test.jpg";

    Mesh::Import(assets/"models/zelensky.glb", [](auto &&mesh, std::string_view name, pose pose) {
        printf(" loading mesh %*s\n", (int) name.size(), name.cbegin());
        auto &imesh = World::meshes.push(std::move(mesh));
        imesh->material = &gamemat;
        imesh->Load();

        Physics::Body& body = World::bodies.push();
        body.SetMass(1);
        body.meshid = imesh.GetId();
        body.pose = pose; 

        if (name == "propeller") 
            body.avel = vec3{1.f, 0.f, 0.f};
    });
}

void Perekop::OnExit() {
    printf("game's gone\n");
} 