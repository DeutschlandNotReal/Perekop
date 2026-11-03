#define PK_INTERNAL
#include <common.hpp>
#include <PK/Render/renderer.hpp>
#include <PK/Render/lighting.hpp>
#include <PK/Physics/body.hpp>
#include <PK/Scene/world.hpp>
using namespace pk;
using namespace Perekop;


void Draw(const Model& model, const mat4& camera) noexcept {
    using namespace Render;
    const Mesh &mesh = World::meshes[model.meshid];

    Shader &shader = mesh.material->shader;
    shader.Use();
    shader.GetUniform("camera")   = camera;
    shader.GetUniform("lightPos") = Lighting::Light.pos;
    shader.GetUniform("model")    = (mat4) model.pose;
    shader.GetUniform("scale")    = model.scale;

    shader.SetTexture("T", 0, mesh.material->textures[0]);

    MeshVAO.Bind();
    MeshVAO.BindBuffer(0, mesh.VBO, BufferTarget::Array,   sizeof(Mesh::Vertex));
    MeshVAO.BindBuffer(0, mesh.EBO, BufferTarget::Element, 0);
        
    Lighting::Renderer.DrawElems(mesh.ind.size());
}

void Perekop::RenderStep() noexcept {
    using namespace Render;
    PreRender();
    if (!CurrentCamera) return;

    const mat4 VP = CurrentCamera->Projection() * CurrentCamera->View();
    Lighting::Renderer.Viewport();
    Lighting::Renderer.Fill({.15f, .15f, .15f});
    Lighting::Renderer.Clear((int)Render::ClearTarget::Depth | (int)Render::ClearTarget::Colour);

    for (const Model &model : World::models)
        Draw(model, VP);

    for (const Body &body : World::bodies)
        Draw(body, VP);

    Lighting::Renderer.Swap();
    PostRender();
}