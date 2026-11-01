#define PK_INTERNAL
#include <common.hpp>
#include <PK/Render/renderer.hpp>
#include <PK/Render/lighting.hpp>
#include <PK/Physics/body.hpp>
#include <PK/Scene/world.hpp>
using namespace pk;
using namespace Perekop;


void Draw(pose pose, uint meshid, vec3 scale, const mat4& camera) noexcept {
    using namespace Render;
    const Mesh &mesh = World::meshes[meshid];

    Shader &shader = mesh.material->shader;
    shader.Use();
    shader.GetUniform("camera")   = camera;
    shader.GetUniform("lightPos") = Lighting::Light.pos;
    shader.GetUniform("model")    = (mat4) pose;
    shader.GetUniform("scale")    = scale;

    shader.SetTexture("T", 0, mesh.material->textures[0]);

    MeshVAO.Bind();
    MeshVAO.BindBuffer(0, mesh.VBO, BufferTarget::Array,   sizeof(Mesh::Vertex));
    MeshVAO.BindBuffer(0, mesh.EBO, BufferTarget::Element, 0);
        
    Lighting::Renderer.DrawElems(mesh.ind.size());
}

void Perekop::RenderStep() noexcept {
    using namespace Render;

    const mat4 VP = Lighting::Camera.Projection() * Lighting::Camera.View();
    Lighting::Renderer.Viewport();
    Lighting::Renderer.Fill({.15f, .15f, .15f});
    Lighting::Renderer.Clear((int)Render::ClearTarget::Depth | (int)Render::ClearTarget::Colour);

    for (const Model &model : World::models)
        Draw(model.pose, model.meshid, model.scale, VP);

    for (const Physics::Body &body : World::bodies)
        Draw(body.pose, body.meshid, body.scale, VP);

    Lighting::Renderer.Swap();
}