#include "gl.hpp"
#include "mesh.hpp"
#include "util.hpp"

using glm::vec3, glm::vec2;
using uint = unsigned int;

uint load_shader(const char* src, GLenum type) {
    // should probably make it detect sloppy shader code
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    return shader;
}

namespace pk {
    Mesh* MeshRenderer::create_mesh() {
        Mesh* mesh = new Mesh(meshes.size());
        meshes.push_back(mesh);
        glGenVertexArrays(1, &mesh->VAO);
        glGenBuffers(1, &mesh->VBO);
        glGenBuffers(1, &mesh->EBO);
        glGenBuffers(1, &mesh->IBO);
        return mesh;
    }

    MeshMaterial MeshRenderer::create_material(const char* vsrc, const char* fsrc) {
        uint vshader = load_shader(vsrc, GL_VERTEX_SHADER);
        
    }

    void Mesh::flush() {
        glBindVertexArray(VAO);

        // position and uv
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(MeshVertex), verts.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)12);
        glEnableVertexAttribArray(1);

        // triangle...
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, trigs.size() * sizeof(MeshTrig), trigs.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    } 

    uint16 Mesh::push_triangle(uint16 v0, uint16 v1, uint16 v2) {
        trigs.emplace_back(v0, v1, v2);
        return trigs.size() - 1;
    }

    uint16 Mesh::pop_triangle() {
        uint16 tid = trigs.size() - 1;
        trigs.pop_back();
        return tid;
    }

    void Mesh::set_verticies(uint16 tid, uint16 v0, uint16 v1, uint16 v2) {
        trigs[tid] = { v0, v1, v2 };
    }

    uint16 Mesh::push_vertex(vec3 pos, vec2 uv) {
        verts.emplace_back(pos, uv);
        return verts.size() - 1;
    }

    // will explode pc if you remove verticies used in trigs
    uint16 Mesh::pop_vertex() {
        uint16 vid = verts.size() - 1;
        verts.pop_back();
        return vid;
    }

    void Mesh::set_position(uint16 vid, vec3 pos) { verts[vid].pos = pos; }

    void Mesh::set_uv(uint16 vid, vec2 uv) { verts[vid].uv = uv; }

    void Mesh::clear_geometry() {
        verts.clear();
        trigs.clear();
        flush();
    }

    // sounds cooler than unassign
    void Mesh::disassign_model(Model* model) {
        if (model->mesh != this) return;
        pk::util::swappop<Model*>(users, model->index, [](auto& V, auto I){ V->index = I; });
        model->mesh = nullptr;
    }

    void Mesh::assign_model(Model* model) {
        if (model->mesh == this) return;
        if (model->mesh) { model->mesh->disassign_model(model); }
        users.push_back(model);
        model->mesh = this;
        model->index = users.size() - 1;
    }

    Mesh::~Mesh() {
        for (Model* user : users) { user->mesh = nullptr; }
    }

    void Model::set_mesh(Mesh* newMesh) {
        if (mesh == newMesh) return;
        newMesh->assign_model(this);
    }

    inline Mesh* Model::get_mesh() { return mesh; }
}