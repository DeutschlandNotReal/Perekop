#include <PK/Connections/step.hpp>
#include <PK/Render/lighting.hpp>
#include <PK/Render/window.hpp>
#include <PK/Scene/scene.hpp>
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
        pk::World::meshes.push(
            assets / "models/Untitled.glb",
            assets / "models/PK67.glb"
        );
        for (pk::Mesh& mesh : pk::World::meshes)
            mesh.Load();

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

        for (pk::Mesh& mesh : pk::World::meshes)
            mesh.material = &gameMaterial;
    }

    void CreateWorld() {
        pk::World::models.emplace(pk::Model{
            .meshid = 0,
            .pose = pk::pose{{0, 0, 0}},
            .scale = glm::vec3{2.5f, 2.5f, 2.5f}
        });

        constexpr unsigned count = 10;
        constexpr float radius = 12.f;
        for (unsigned i = 0; i < count; ++i) {
            const float angle = 6.2831853f * (float)i / (float)count;
            const glm::vec3 position{
                std::cos(angle) * radius,
                0,
                std::sin(angle) * radius
            };
            const glm::quat rotation = glm::angleAxis(angle, glm::vec3{0, 1, 0});

            pk::World::models.emplace(pk::Model{
                .meshid = 1,
                .pose = pk::pose{position, rotation},
                .scale = glm::vec3{1.5f, 1.5f + (float)(i % 3) * .35f, 1.5f}
            });
        }
    }
}

void Perekop::OnStep(double dt) {
    PKG::StepCamera(pk::Lighting::Camera, dt);
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