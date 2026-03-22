#pragma once
#include "glm/glm.hpp"
#include <vector>
#include <functional>

using uint16 = unsigned short;

struct PKMeshVertex {
     glm::vec3 pos; glm::vec2 uv; 
     PKMeshVertex(glm::vec3 POS, glm::vec2 UV): pos(POS), uv(UV) {}
};

// turbopacked slop
struct __attribute__((packed)) PKMeshTrig {
    uint16 v0, v1, v2;
    PKMeshTrig(uint16 V0, uint16 V1, uint16 V2): v0(V0), v1(V1), v2(V2) {} 
};

struct PKMesh;
struct PKModel;

struct PKRenderer {
    friend PKMesh;
    private:
        std::vector<PKMesh*> meshes;
    public:
        void draw();
        void draw_tested(std::function<bool(PKMesh&, PKModel&)>);
        PKMesh* create_mesh();
};

struct PKMesh {
    friend PKModel;
    friend PKRenderer;
    private:
        PKMesh(uint16 i);
        uint16 index;
        std::vector<PKMeshVertex> verts;
        std::vector<PKMeshTrig> trigs;
        unsigned int VAO, VBO, EBO, IBO;
        std::vector<PKModel*> users;
    public:
        PKMesh() = delete;
        ~PKMesh();
        uint16 pop_triangle();
        uint16 push_triangle(uint16 v0, uint16 v1, uint16 v2);
        uint16 pop_vertex();
        uint16 push_vertex(glm::vec3 pos, glm::vec2 uv = glm::vec2(0, 0));
        void set_position(uint16 vid, glm::vec3 pos);
        void set_uv(uint16 vid, glm::vec2 uv);
        void set_verticies(uint16 tid, uint16 v0, uint16 v1, uint16 v2);

        void clear_geometry();

        void flush();

        void disassign_model(PKModel* M);
        void assign_model(PKModel* M);
};

struct PKModel {
    friend PKMesh;
    friend PKRenderer;
    private:
        uint16 index;
        PKMesh* mesh;
    public:
        glm::mat3x4 matrix;
        glm::vec3 size;
        bool visible;
        ~PKModel();
        void set_mesh(PKMesh* mesh);
        inline PKMesh* get_mesh();
};