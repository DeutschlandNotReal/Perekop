#include "gl.hpp"
#include "mesh.hpp"

PKMesh* PKRenderer::initMesh() {
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

uint16 PKMesh::pushTrig(uint16 v0, uint16 v1, uint16 v2) {
    trigs.emplace_back(v0, v1, v2);
    return trigs.size() - 1;
}

uint16 PKMesh::popTrig() {
    uint16 tid = trigs.size() - 1;
    trigs.pop_back();
    return tid;
}

void PKMesh::setTrig(uint16 tid, uint16 v0, uint16 v1, uint16 v2) {
    trigs[tid] = { v0, v1, v2 };
}

uint16 PKMesh::pushVert(glm::vec3 pos, glm::vec2 uv) {
    verts.emplace_back(pos, uv);
    return verts.size() - 1;
}

// will explode pc if you remove verticies used in trigs
uint16 PKMesh::popVert() {
    uint16 vid = verts.size() - 1;
    verts.pop_back();
    return vid;
}

void PKMesh::setPosition(uint16 vid, glm::vec3 pos) { verts[vid].pos = pos; }

void PKMesh::setUV(uint16 vid, glm::vec2 uv) { verts[vid].uv = uv; }

void PKMesh::clearGeometry() {
    verts.clear();
    trigs.clear();
    flush();
}

void PKMesh::removeModel(PKModel* model) {
    if (model->mesh != this) return;
    PKModel* back = users.back();
    if (back != model) { back->index = model->index; }
    users[model->index] = back;
    users.pop_back();
    model->mesh = nullptr;
}

void PKMesh::addModel(PKModel* model) {
    if (model->mesh == this) return;
    if (model->mesh) { model->mesh->removeModel(model); }
    users.push_back(model);
    model->mesh = this;
    model->index = users.size() - 1;
}

PKMesh::~PKMesh() {
    for (PKModel* user : users) { user->mesh = nullptr; }
}

void PKModel::setMesh(PKMesh* newMesh) {
    if (mesh == newMesh) return;
    newMesh->addModel(this);
}

inline PKMesh* PKModel::getMesh() { return mesh; }