#define PK_INTERNAL
#include <common.hpp>
#include <PK/Render/renderer.hpp>
#include <PK/Render/lighting.hpp>
#include <PK/Physics/body.hpp>
#include <PK/Scene/world.hpp>
using namespace pk;
using namespace Perekop;

template <bool shadow>
void DrawModel(const Model& model, const mat4& camera, const mat4& light) noexcept {
    const Mesh&  mesh  = World::meshes[model.meshid];

    Render::Shader& shader = shadow ? Lighting::Shadow::Shader : mesh.material->shader;
    shader.Use();
    shader.GetUniform("camera")   = camera;
    shader.GetUniform("light")    = light;
    shader.GetUniform("lightPos") = Lighting::Light.pos;
    shader.GetUniform("model")    = (mat4) model.pose;
    shader.GetUniform("scale")    = model.scale;

    if constexpr (!shadow) {
        shader.SetTexture("T",      0, mesh.material->textures[0]);
        shader.SetTexture("shadow", 1, Lighting::Shadow::Texture);
    }

    MeshVAO.Bind();
    MeshVAO.BindBuffer(0, mesh.VBO, Render::BufferTarget::Array,   sizeof(Mesh::Vertex));
    MeshVAO.BindBuffer(0, mesh.EBO, Render::BufferTarget::Element, 0);
        
    Lighting::Renderer.DrawElems(3 * mesh.t.size());
}

void Perekop::RenderStep() noexcept {
    const mat4 VP = Lighting::Camera.Projection() * Lighting::Camera.View();
    const mat4 L  = Lighting::Light;

    Lighting::Renderer.Target(Lighting::Shadow::Buffer);
    Lighting::Renderer.Fill({1.f, 1.f, 1.f});
    Lighting::Renderer.Clear((int)Render::ClearTarget::Depth);

    // shadow stage
    for (const Model& model : World::models) 
        DrawModel<true>(model, L, L);
    for (const Physics::Body& body : World::bodies)
        DrawModel<true>(body.model, L, L);

    Lighting::Renderer.Viewport();
    Lighting::Renderer.Fill({.15f, .15f, .15f});
    Lighting::Renderer.Clear((int)Render::ClearTarget::Depth | (int)Render::ClearTarget::Colour);

    for (const Model& model : World::models) 
        DrawModel<false>(model, VP, L);
    for (const Physics::Body& body : World::bodies)
        DrawModel<false>(body.model, VP, L);

    Lighting::Renderer.Swap();
}