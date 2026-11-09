#define PK_INTERNAL
#include "./internal.hpp"

#include <PK/Render/renderer.hpp>
#include <PK/Render/lighting.hpp>
#include <PK/Physics/body.hpp>
#include <PK/Render/window.hpp>
#include <PK/world.hpp>

using namespace pk;
using namespace Perekop;

namespace {
    void draw(const model& model, const mat4& camera) noexcept {
        const mesh& mesh = world::meshes[model.meshid];

        shader& shader = mesh.material->shader;
        shader.use();
        shader.getUniform("camera")   = camera;
        shader.getUniform("lightPos") = lighting::light.pos;
        shader.getUniform("model")    = (mat4) model.pose;
        shader.getUniform("scale")    = model.scale;
        shader.getUniform("tint")     = model.tint;

        shader.setTexture("T", 0, mesh.material->textures[0]);

        meshVAO.bind();
        meshVAO.bindBuffer(0, mesh.VBO, bufferTar::array,   sizeof(mesh::vertex));
        meshVAO.bindBuffer(0, mesh.EBO, bufferTar::element, 0);

        lighting::renderer.drawElems(mesh.ind.size());
    }
}

void Perekop::renderStep() noexcept {
    if (!mainWindow.mainCamera) return;

    const mat4 viewProjection = mainWindow.mainCamera->projection() * mainWindow.mainCamera->view();
    lighting::renderer.viewport();
    lighting::renderer.fill({.15f, .15f, .15f});
    lighting::renderer.clear((int)clearTar::depth | (int)clearTar::colour);

    for (const model& model : world::models)
        draw(model, viewProjection);

    for (const body& body : world::bodies)
        draw(body, viewProjection);

    lighting::renderer.swap();
}