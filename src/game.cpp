#include <PK/Connections/step.hpp>
#include <PK/Util/file.hpp>
#include <PK/Render/window.hpp>
#include <PK/Render/renderer.hpp>
#include <PK/Connections/userinput.hpp>
#include <PK/Physics/collision.hpp>
#include <cstdio>
#include <PK/Systems/camera.hpp>
#include <PK/Util/time.hpp>

#include <PK/Connections/collections.hpp>
#include <PK/Render/lighting.hpp>

using namespace pk;
using namespace pk::Render;
using namespace Perekop;
const path assets = "assets";
Render::Shader GameShader;

Material GameMaterial;

void Perekop::OnStep(double dt) {
    PKG::StepCamera(Lighting::Camera, dt);
}

void Perekop::OnRender() {

}

void Perekop::OnLaunch() {
    printf("Game begin\n");
    Window::SetIcon(assets / "images/icon.ico");

    Lighting::Light = pose::LookAt({50, 36, 22}, {0, 2, 0});
    PKG::InitCamera(Lighting::Camera);

    Lighting::Shadow::Texture = Texture::Depth(2048, 2048);
    Lighting::Shadow::Buffer = Framebuffer(2048, 2048);
    Lighting::Shadow::Buffer.AttachDepth(Lighting::Shadow::Texture);

    World::meshes.push(  
        assets / "models/Untitled.glb",
        assets / "models/PK67.glb"
    );

    for (Mesh& mesh : World::meshes) mesh.Load();

    Lighting::Shadow::Shader = Shader({
        ShaderStage(ShaderType::Vertex, assets / "shaders/vert.glsl"),
        ShaderStage(ShaderType::Fragment, (std::string_view) "#version 430\n void main() {}")

    });
    GameShader = Shader({
        ShaderStage(ShaderType::Vertex, assets / "shaders/vert.glsl"),
        ShaderStage(ShaderType::Fragment, assets / "shaders/frag.glsl")
    });

    Lighting::Material = Texture(assets / "images/test.jpg");

    GameMaterial.shader = std::move(GameShader);
    GameMaterial.textures[0] = Lighting::Material;
    for (Mesh& mesh : World::meshes) mesh.material = &GameMaterial;

    World::collections.emplace();
    for (unsigned i = 0; i < World::meshes.size(); ++i)
        World::collections[0].AddModel({
            .pose = vec3{i * 6, 0, 0},
            .scale = vec3{1, i, 1}
        }, i);
}

void Perekop::OnExit() {
    printf("game's gone\n");
} 