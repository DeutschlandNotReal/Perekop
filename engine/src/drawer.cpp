#define PK_INTERNAL
#include <PKINT/internal.hpp>
#include <PK/shader.hpp>
#include <PK/world.hpp>
#include <PK/lighting.hpp>
using namespace pk;
using namespace Perekop;

inline pk::vector<pk::vector<Render::RenderModel>> rmodels;
void Perekop::RenderStep() noexcept {
    rmodels.reserve(World::meshes.size());
    for (int i = World::meshes.size(); i < rmodels.size(); i++) {
        rmodels[i].emplace();
        rmodels[i].clear();
    }

    for (const Model& model : World::models) 
        rmodels[model.meshid].emplace(model.pose, model.scale, model.metadata);

    Lighting::ShaderData::CameraMatrix = Lighting::Camera.Projection() * Lighting::Camera.View();
    Lighting::ShaderData::LightMatrix  = Lighting::Light;

    Lighting::Renderer.Target(Lighting::Shadow::Buffer, Render::DrawTarget::None);
    Lighting::Renderer.Fill({1.f, 1.f, 1.f});
    Lighting::Renderer.Clear(DepthBuffer);

    // shadow run
    for (int i = 0; i < World::meshes.size(); i++) {
        Lighting::Renderer.Draw(Lighting::Shadow::Shader, World::meshes[i], rmodels[i]);
    }

    Lighting::Renderer.Viewport();
    Lighting::Renderer.Fill({.15f, .15f, .15f});
    Lighting::Renderer.Clear(DepthBuffer | ColourBuffer);

    for (int i = 0; i < World::meshes.size(); i++) {
        const Mesh& mesh = World::meshes[i];
        Lighting::Renderer.Draw(*mesh.shader, mesh, rmodels[i]);
    }

    Lighting::Renderer.Swap();
}  
