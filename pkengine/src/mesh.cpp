#include "gl.hpp"
#include "mesh.hpp"

PKMesh* PKRenderer::create_mesh() {
    auto* mesh = new PKMesh(meshes.size());
    meshes.push_back(mesh);
    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    glGenBuffers(1, &mesh->EBO);
    glGenBuffers(1, &mesh->IBO);
    return mesh;
}

void PKMesh::flush() {
    glBindVertexArray(VAO);

    // position and uv
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(PKMeshVertex), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PKMeshVertex), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(PKMeshVertex), (void*)12);
    glEnableVertexAttribArray(1);

    // triangle...
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, trigs.size() * sizeof(PKMeshTrig), trigs.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
} 

uint16 PKMesh::push_triangle(uint16 v0, uint16 v1, uint16 v2) {
    trigs.emplace_back(v0, v1, v2);
    return trigs.size() - 1;
}

uint16 PKMesh::pop_triangle() {
    uint16 tid = trigs.size() - 1;
    trigs.pop_back();
    return tid;
}

void PKMesh::set_verticies(uint16 tid, uint16 v0, uint16 v1, uint16 v2) {
    trigs[tid] = { v0, v1, v2 };
}

uint16 PKMesh::push_vertex(glm::vec3 pos, glm::vec2 uv) {
    verts.emplace_back(pos, uv);
    return verts.size() - 1;
}

// will explode pc if you remove verticies used in trigs
uint16 PKMesh::pop_vertex() {
    uint16 vid = verts.size() - 1;
    verts.pop_back();
    return vid;
}

void PKMesh::set_position(uint16 vid, glm::vec3 pos) { verts[vid].pos = pos; }

void PKMesh::set_uv(uint16 vid, glm::vec2 uv) { verts[vid].uv = uv; }

void PKMesh::clear_geometry() {
    verts.clear();
    trigs.clear();
    flush();
}

// is disassign even a word
// maybe its deassign, maybe
void PKMesh::disassign_model(PKModel* model) {
    if (model->mesh != this) return;
    PKModel* back = users.back();
    if (back != model) { back->index = model->index; }
    users[model->index] = back;
    users.pop_back();
    model->mesh = nullptr;
}

void PKMesh::assign_model(PKModel* model) {
    if (model->mesh == this) return;
    if (model->mesh) { model->mesh->disassign_model(model); }
    users.push_back(model);
    model->mesh = this;
    model->index = users.size() - 1;
}

PKMesh::~PKMesh() {
    for (PKModel* user : users) { user->mesh = nullptr; }
}

void PKModel::set_mesh(PKMesh* newMesh) {
    if (mesh == newMesh) return;
    newMesh->assign_model(this);
}

inline PKMesh* PKModel::get_mesh() { return mesh; }