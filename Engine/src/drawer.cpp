#define PK_INTERNAL
#include "./internal.hpp"

#include <PK/Render/Renderer.hpp>
#include <PK/Render/lighting.hpp>
#include <PK/Physics/Body.hpp>
#include <PK/Render/window.hpp>
#include <PK/world.hpp>
#include <PK/Render/Resources/material.hpp>

#include <PK/Math/format.hpp>

#include <glad/glad.h>
#include <glfw/glfw3.h>

using namespace pk;
using namespace Perekop;

namespace {
    void draw(const ModelHandle& model, const mat4& Camera) noexcept {
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

        Shader& shader = mesh.material->shader;
        shader.use();
        shader.getUniform("camera") = Camera;
        shader.getUniform("model")  = (mat4) model.pose;
        shader.getUniform("scale")  = model.scale;

        shader.setTexture("T", 0, mesh.material->textures[0]);

        meshVAO.bind();
        meshVAO.bindBuffer(0, mesh.VBO, BufferTar::array,   sizeof(Mesh::Vertex));
        meshVAO.bindBuffer(0, mesh.EBO, BufferTar::element, 0);

        lighting::Renderer.drawElems(mesh.ind.size());
    }

    vector<Wirebox> wireboxes;
    
   void drawModelBoxes() {
        wireboxes.clear();
        
        for (const ModelHandle& model : ModelHandle::items) {
            AABB b = model.getWorldAABB();
            wireboxes.emplace(Wirebox{
                mat3{1.f}, 
                b.getExtents(), 
                b.getCentre(), 
                vec4{0, 0, 1, .7f}}
            );
        }

        lighting::Renderer.drawWireboxes(span<Wirebox>{wireboxes});
    }

    void drawBVH(int maxdepth, bool leafonly) {
        wireboxes.clear();
        auto nodes = Perekop::ModelBVH.getNodes();
        if (!nodes.size()) return;

        struct Entry { int id, depth; } stack[128];
        int top{0};
        stack[top++] = {0, 0};

        float rmaxd = 1.f / (float)max(maxdepth, 1);

        while (top) {
            auto [id, depth] = stack[--top];
            const auto& n = nodes[id];
            AABB b = n.bounds;

            float t = depth * rmaxd;

            static constexpr vec4 colHDepth = {1.f, 1.f, 1.f, .1f}; 
            static constexpr vec4 colLDepth = {1.f, 1.f, 1.f, .05f};
            static constexpr vec4 redcol    = {1.f, 0.f, 0.f, .05f};

            vec4 colour = n.leaf ? mix(colLDepth, redcol, (n.lhs - n.rhs) / -5.f) : mix(colHDepth, colLDepth, t);

            if (depth == maxdepth || !leafonly || n.leaf ) wireboxes.emplace(
                Wirebox{
                    mat3{1.f}, 
                    b.getExtents(), 
                    b.getCentre(), 
                    colour
                }
            );

            if (n.leaf || (depth >= maxdepth) && !leafonly) continue;
      
            stack[top++] = {n.rhs, depth + 1};
            stack[top++] = {n.lhs, depth + 1};
        }

        lighting::Renderer.drawWireboxes(span<Wirebox>{wireboxes});
    }
}


int   MaxBVH  = 3;
bool  LeafBVH = false;
void init::drawer() {
    Window::main.onKeyDown.bind([](int key) {
        switch (key) {
            case 'I': ++MaxBVH; break;
            case 'O': --MaxBVH; break;
            case 'P': LeafBVH = !LeafBVH; break;
        }

        MaxBVH = std::clamp(MaxBVH, 0, 12);
    });
}

void step::drawer(float dt) {
    if (!Window::main.camera) {
        warn("Where the camera at?");
        return;
    };

    lighting::Renderer.setMode(RasterMode::fill);
    const mat4 VP = Window::main.camera->projection() * Window::main.camera->view();
    lighting::Renderer.viewport();
    lighting::Renderer.fill({.15f, .15f, .15f});
    lighting::Renderer.clear((int)ClearTar::depth | (int)ClearTar::colour);

    for (const ModelHandle& model : ModelHandle::items)
        draw(model, VP);

    drawModelBoxes();
    drawBVH(MaxBVH, LeafBVH);

    lighting::Renderer.swap();
}