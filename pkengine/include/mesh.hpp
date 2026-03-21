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
        void drawTest(std::function<bool(PKMesh&, PKModel&)>);
        PKMesh* initMesh();
};

struct PKMesh {
    friend PKModel;
    friend PKRenderer;
    using GLuint = unsigned int;
    private:
        PKMesh(uint16 i);
        uint16 index;
        std::vector<PKMeshVertex> verts;
        std::vector<PKMeshTrig> trigs;
        GLuint VAO, VBO, EBO, IBO;
        std::vector<PKModel*> users;
    public:
        PKMesh() = delete;
        ~PKMesh();
        uint16 popTrig();
        uint16 pushTrig(uint16 v0, uint16 v1, uint16 v2);
        uint16 popVert();
        uint16 pushVert(glm::vec3 pos, glm::vec2 uv = glm::vec2(0, 0));
        void setPosition(uint16 vid, glm::vec3 pos);
        void setUV(uint16 vid, glm::vec2 uv);
        void setTrig(uint16 tid, uint16 v0, uint16 v1, uint16 v2);

        void clearGeometry();

        void flush();

        void removeModel(PKModel* M);
        void addModel(PKModel* M);
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
        void setMesh(PKMesh* mesh);
        inline PKMesh* getMesh();
};