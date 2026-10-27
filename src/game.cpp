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

void CreateWorld() {
    for (uint i{0}; i < 2; ++i) {
        Physics::Body& body = World::bodies.push();
        
        body.meshid = 1;
        body.shapetype    = Physics::ShapeType::Ball;
        
        body.pose = vec3{(float)(rand() % 100), (float)(rand() % 100), (float)(rand() % 100)};

        body.scale = vec3{1.f};

        body.SetMass(1.f);

        body.vel = (i%2==0) ? vec3{1, 1, 1} : vec3{-1, -1, -1};
        body.vel *= .5f;
    }

    World::joints.push(
        Physics::Joint::SpringType(2, 5)
            .AttachA(1, vec3{1, 2, 1})
            .AttachB(2, vec3{4, 3, -1})
    );
}


void Perekop::OnStep(double dt) {
    PKG::StepCamera(pk::Lighting::Camera, dt);
    Perekop::PhysicsStep(dt);
}

void Perekop::OnRender() {

}

void Perekop::OnLaunch() {
    printf("Game begin\n");
    Window::SetIcon(assets/"images/icon.ico");

    using namespace Lighting;
    using namespace Render;
    Light = pk::pose::LookAt({50, 36, 22}, {0, 2, 0});
    Lighting::Camera.pose = pk::pose::LookAt({0, 12, 30}, {0, 2, 0});
    PKG::InitCamera(pk::Lighting::Camera);

    Shadow::Texture = Texture::Depth(2048, 2048);
    Shadow::Buffer = Framebuffer(2048, 2048);
    Shadow::Buffer.AttachDepth(Shadow::Texture);

    Shadow::Shader = Render::Shader({
        {ShaderType::Vertex, assets/"shaders/vert.glsl"},
        {ShaderType::Fragment, (std::string_view) "#version 430\n void main() {}"}
    });

    gamemat.shader = pk::Render::Shader({
        {ShaderType::Vertex,   assets/"shaders/vert.glsl"},
        {ShaderType::Fragment, assets/"shaders/frag.glsl"}
    }); 
        
    gamemat.textures[0] = Lighting::Material = assets/"images/test.jpg";

    Mesh& box = World::meshes.push(assets/"models/PK67.glb");
    box.material = &gamemat;
    box.Load();
    CreateWorld();
}

void Perekop::OnExit() {
    printf("game's gone\n");
} 