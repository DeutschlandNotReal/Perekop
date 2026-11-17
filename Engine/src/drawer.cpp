#define PK_INTERNAL
#include "./internal.hpp"

#include <PK/Render/Renderer.hpp>
#include <PK/Physics/Body.hpp>
#include <PK/Render/window.hpp>
#include <PK/world.hpp>
#include <PK/Render/Resources/material.hpp>
#include <PK/Math/format.hpp>
#include <PK/tick.hpp>
#include <PK/Util/time.hpp>

#include <glad/glad.h>
#include <glfw/glfw3.h>

using namespace pk;
using namespace Perekop;

static Renderer renderer;
int drawcount{0};

namespace {
    void draw(const ModelHandle& model, const mat4& VP) noexcept {
        const MeshHandle* meshptr = model.meshID.get();

        if (!meshptr) {
            warn(std::format("Model {} missing mesh", (uint)model.getID()));
            return;
        }

        const MeshHandle& mesh = *meshptr;

        if (!mesh.material) {
            warn(std::format("Mesh {} missing a material", (uint)mesh.getID()));
            return;
        }

        if (!mesh.loaded()) {
            warn(std::format("Mesh {} isn't even loaded son", (uint)mesh.getID()));
            return;
        }

        ++drawcount;

        Shader& shader = mesh.material->shader;
        shader.use();
        shader.getUniform("MVP") = VP * (mat4) model.pose;
        shader.getUniform("scale")  = model.scale;

        shader.setTexture("T", 0, mesh.material->textures[0]);

        Mesh::VAO.bindBuffer(0, mesh.VBO, BufferTar::array,   sizeof(Mesh::Vertex));
        Mesh::VAO.bindBuffer(0, mesh.EBO, BufferTar::element, 0);

        renderer.draw(DrawMode::element, mesh.ind.size());
    }
}


void Perekop::initDrawer() {

}

void Perekop::stepDrawer() {
    drawcount = 0;
    if (!Window::main.camera) {
        warn("Camera missing");
        return;
    };

    renderer.setMode(RasterMode::fill);

    const mat4 VP = Window::main.camera->projection() * Window::main.camera->view();

    time::Tracker<float> drawtime;
    drawtime.begin();
    renderer.viewport();
    renderer.fill({.15f, .15f, .15f});
    renderer.clear((int)ClearTar::depth | (int)ClearTar::colour);

    
    Mesh::VAO.bind();
    for (const ModelHandle& model : ModelHandle::items)
        draw(model, VP);

    float t = drawtime.stop();

    for (GLenum e; (e = glGetError()) != GL_NO_ERROR;) warn(std::format("GL error 0x{:X}", e));

    Perekop::onRender();
    renderer.swap();
}