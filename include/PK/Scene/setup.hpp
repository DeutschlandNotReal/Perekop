#pragma once

#include <PK/Core/vector.hpp>
#include <PK/Math/pose.hpp>
#include <PK/Render/lighting.hpp>
#include <PK/Render/Resources/material.hpp>
#include <PK/Render/Resources/shader.hpp>
#include <PK/Scene/collections.hpp>
#include <PK/Systems/camera.hpp>

#include <cmath>
#include <filesystem>
#include <utility>

namespace pk::Scene {
    inline Render::Shader gameShader;
    inline Material gameMaterial;

    inline void Initialize(const std::filesystem::path& assets) {
        Lighting::Light = pose::LookAt({50, 36, 22}, {0, 2, 0});
        Lighting::Camera.pose = pose::LookAt({0, 12, 30}, {0, 2, 0});
        PKG::InitCamera(Lighting::Camera);

        Lighting::Shadow::Texture = Render::Texture::Depth(2048, 2048);
        Lighting::Shadow::Buffer = Render::Framebuffer(2048, 2048);
        Lighting::Shadow::Buffer.AttachDepth(Lighting::Shadow::Texture);

        World::meshes.push(
            assets / "models/Untitled.glb",
            assets / "models/PK67.glb"
        );
        for (Mesh& mesh : World::meshes) mesh.Load();

        Lighting::Shadow::Shader = Render::Shader({
            Render::ShaderStage(Render::ShaderType::Vertex, assets / "shaders/vert.glsl"),
            Render::ShaderStage(Render::ShaderType::Fragment, (std::string_view) "#version 430\n void main() {}")
        });
        gameShader = Render::Shader({
            Render::ShaderStage(Render::ShaderType::Vertex, assets / "shaders/vert.glsl"),
            Render::ShaderStage(Render::ShaderType::Fragment, assets / "shaders/frag.glsl")
        });

        Lighting::Material = Render::Texture(assets / "images/test.jpg");
        gameMaterial.shader = std::move(gameShader);
        gameMaterial.textures[0] = Lighting::Material;
        for (Mesh& mesh : World::meshes) mesh.material = &gameMaterial;

        World::collections.emplace();
        Collection& collection = World::collections[0];

        collection.AddModel({
            .pose = pose{{0, 0, 0}},
            .scale = vec3{2.5f, 2.5f, 2.5f}
        }, 0);

        constexpr unsigned count = 10;
        constexpr float radius = 12.f;
        for (unsigned i = 0; i < count; ++i) {
            const float angle = 6.2831853f * (float)i / (float)count;
            const vec3 position{
                std::cos(angle) * radius,
                0,
                std::sin(angle) * radius
            };
            const quat rotation = angleAxis(angle, vec3{0, 1, 0});

            collection.AddModel({
                .pose = pose{position, rotation},
                .scale = vec3{1.5f, 1.5f + (float)(i % 3) * .35f, 1.5f}
            }, 1);
        }
    }
}
