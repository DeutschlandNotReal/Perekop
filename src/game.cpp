#define PK_INTERNAL
#include <common.hpp>
#include <PK/Connections/step.hpp>
#include <PK/Render/lighting.hpp>
#include <PK/Render/window.hpp>
#include <PK/Scene/world.hpp>
#include <PK/Systems/camera.hpp>

#include <cmath>
#include <cstdio>

using namespace Perekop; 

namespace {
    const std::filesystem::path assets = "assets";
    pk::Material gameMaterial;

    void SetupCameraAndLighting() {
        pk::Lighting::Light = pk::pose::LookAt({50, 36, 22}, {0, 2, 0});
        pk::Lighting::Camera.pose = pk::pose::LookAt({0, 12, 30}, {0, 2, 0});
        PKG::InitCamera(pk::Lighting::Camera);

        pk::Lighting::Shadow::Texture = pk::Render::Texture::Depth(2048, 2048);
        pk::Lighting::Shadow::Buffer = pk::Render::Framebuffer(2048, 2048);
        pk::Lighting::Shadow::Buffer.AttachDepth(pk::Lighting::Shadow::Texture);
    }

    void LoadGameAssets() {
        pk::Lighting::Shadow::Shader = pk::Render::Shader({
            pk::Render::ShaderStage(pk::Render::ShaderType::Vertex, assets / "shaders/vert.glsl"),
            pk::Render::ShaderStage(pk::Render::ShaderType::Fragment, (std::string_view) "#version 430\n void main() {}")
        });

        gameMaterial.shader = pk::Render::Shader({
            pk::Render::ShaderStage(pk::Render::ShaderType::Vertex, assets / "shaders/vert.glsl"),
            pk::Render::ShaderStage(pk::Render::ShaderType::Fragment, assets / "shaders/frag.glsl")
        });
        pk::Lighting::Material = pk::Render::Texture(assets / "images/test.jpg");
        gameMaterial.textures[0] = pk::Lighting::Material;

        const glm::vec3 normal[6]{
            {0, 0, 1}, {0, 0, -1}, {1, 0, 0},
            {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}
        };
        const glm::vec3 pos[6][4]{
            {{-.5f, -.5f,  .5f}, {.5f, -.5f,  .5f}, {.5f,  .5f,  .5f}, {-.5f,  .5f,  .5f}},
            {{ .5f, -.5f, -.5f}, {-.5f, -.5f, -.5f}, {-.5f,  .5f, -.5f}, { .5f,  .5f, -.5f}},
            {{ .5f, -.5f,  .5f}, { .5f, -.5f, -.5f}, { .5f,  .5f, -.5f}, { .5f,  .5f,  .5f}},
            {{-.5f, -.5f, -.5f}, {-.5f, -.5f,  .5f}, {-.5f,  .5f,  .5f}, {-.5f,  .5f, -.5f}},
            {{-.5f,  .5f,  .5f}, { .5f,  .5f,  .5f}, { .5f,  .5f, -.5f}, {-.5f,  .5f, -.5f}},
            {{-.5f, -.5f, -.5f}, { .5f, -.5f, -.5f}, { .5f, -.5f,  .5f}, {-.5f, -.5f,  .5f}}
        };

        pk::Mesh& box = pk::World::meshes.emplace();
        for (unsigned face = 0; face < 6; ++face) {
            unsigned first = box.vtx.size();
            for (unsigned i = 0; i < 4; ++i) {
                pk::Mesh::Vertex& v = box.vtx.emplace();
                v.pos = pos[face][i];
                v.nor = normal[face];
            }
            box.ind.push(first, first + 1, first + 2, first, first + 2, first + 3);
        }
        box.material = &gameMaterial;
        box.Load();
    }

    void CreateWorld() {
        pk::Physics::Body& floor = pk::World::bodies.emplace();
        floor.model.meshid = 0;
        floor.model.pose = pk::pose{{0, -2.5f, 0}};
        floor.model.scale = {16, 1, 16};
        floor.SetFlag(pk::Physics::Body::StateFlag::anchored, true);

        const glm::vec3 wallpos[4]{
            {-7.5f, 2.f, 0.f}, {7.5f, 2.f, 0.f},
            {0.f, 2.f, -7.5f}, {0.f, 2.f, 7.5f}
        };
        const glm::vec3 wallscale[4]{
            {1.f, 8.f, 16.f}, {1.f, 8.f, 16.f},
            {16.f, 8.f, 1.f}, {16.f, 8.f, 1.f}
        };
        for (unsigned i = 0; i < 4; ++i) {
            pk::Physics::Body& wall = pk::World::bodies.emplace();
            wall.model.meshid = 0;
            wall.model.pose = pk::pose{wallpos[i]};
            wall.model.scale = wallscale[i];
            wall.SetFlag(pk::Physics::Body::StateFlag::anchored, true);
        }

        constexpr unsigned count = 200;
        for (unsigned i = 0; i < count; ++i) {
            pk::Physics::Body& body = pk::World::bodies.emplace();
            unsigned row = i / 10, col = i % 10;
            body.model.meshid = 0;
            body.model.pose = pk::pose{
                {((float)col - 4.5f) * 1.2f, 4.f + (float)(i % 3) * 1.2f, ((float)row - 4.5f) * 1.2f},
                glm::angleAxis((float)i * .2f, glm::vec3{0.f, 1.f, 0.f})
            };
            body.model.scale = glm::vec3{1.f};
            body.mass = 1.f;
            body.vel = {
                ((float)(i % 5) - 2.f) * .8f,
                0.f,
                ((float)(i % 7) - 3.f) * .5f
            };
            body.avel = {.7f, ((float)(i % 3) - 1.f) * .6f, .5f};
        }
    }
}

void Perekop::OnStep(double dt) {
    PKG::StepCamera(pk::Lighting::Camera, dt);
    Perekop::PhysicsStep(dt);
}

void Perekop::OnRender() {

}

void Perekop::OnLaunch() {
    printf("Game begin\n");
    Window::SetIcon(assets / "images/icon.ico");
    SetupCameraAndLighting();
    LoadGameAssets();
    CreateWorld();
}

void Perekop::OnExit() {
    printf("game's gone\n");
} 