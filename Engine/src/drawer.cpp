#define PK_INTERNAL
#include "./internal.hpp"

#include <PK/Render/Renderer.hpp>
#include <PK/Render/lighting.hpp>
#include <PK/Physics/Body.hpp>
#include <PK/Render/window.hpp>
#include <PK/world.hpp>

using namespace pk;
using namespace Perekop;

namespace {
    void draw(const Model& Model, const mat4& Camera) noexcept {
        const Mesh& Mesh = *Model.getMesh();

        shader& shader = Mesh.material->shader;
        shader.use();
        shader.getUniform("Camera")   = Camera;
        shader.getUniform("lightPos") = lighting::light.pos;
        shader.getUniform("Model")    = (mat4) Model.pose;
        shader.getUniform("scale")    = Model.scale;

        shader.setTexture("T", 0, Mesh.material->textures[0]);

        meshVAO.bind();
        meshVAO.bindBuffer(0, Mesh.VBO, bufferTar::array,   sizeof(Mesh::Vertex));
        meshVAO.bindBuffer(0, Mesh.EBO, bufferTar::element, 0);

        lighting::Renderer.drawElems(Mesh.ind.size());
    }
}

void step::render() {
    if (!mainWindow.mainCamera) return;

    const mat4 viewProjection = mainWindow.mainCamera->projection() * mainWindow.mainCamera->view();
    lighting::Renderer.viewport();
    lighting::Renderer.fill({.15f, .15f, .15f});
    lighting::Renderer.clear((int)ClearTar::depth | (int)ClearTar::colour);

    for (const ModelHandle& model : ModelHandle::set)
        draw(model, viewProjection);

    lighting::Renderer.swap();
}