#pragma once
#include <glm/glm.hpp>

#include <Perekop/Structure.hpp>
using ID_T = unsigned short;

namespace pk {
    class Model;
    class MeshRenderer;
    class Camera;
    class Mesh;

    struct MeshBounds { glm::vec3 min, max; };

    struct MeshVertex { 
        glm::vec3 pos; glm::vec2 uv;
        MeshVertex(): pos(0), uv(0) {}
        MeshVertex(float x, float y, float z): pos(x, y, z), uv(0) {};
        MeshVertex(float x, float y, float z, float u, float v): pos(x, y, z), uv(u, v) {}
        MeshVertex(glm::vec3 p): pos(p), uv(0) {}
    };

    struct MeshTriangle { 
        ID_T v0, v1, v2; 
        MeshTriangle(ID_T A, ID_T B, ID_T C): v0(A), v1(B), v2(C) {}
    };


    class MeshMaterial {
        friend Mesh;
        friend MeshRenderer;
        unsigned int program;
        void use(const glm::mat4& VP);
        public:
            MeshMaterial(): program(0) {}
            MeshMaterial(const char* vertex_source, const char* fragment_source);
    };

    class Mesh {
        friend Model; friend MeshRenderer;
        unsigned int VAO = 0, VBO = 0, EBO = 0, IBO = 0;
        ID_T meshid = 0;
        MeshRenderer* renderer;

        pk::Array<Model*> users;

        ~Mesh();
        public:
            pk::Array<MeshVertex> vertex;
            pk::Array<MeshTriangle> triangle;
            Mesh(MeshMaterial mat): material(mat) {}
            MeshMaterial material;

            MeshBounds bounds() const noexcept;
            bool is_loaded() const noexcept;

            void load();
            void unload();
            void refresh();
    };

    class MeshRenderer {
        friend Mesh;

        pk::Array<Mesh*> meshes;
        pk::Array<glm::mat3x4> transforms;

        public:
            Mesh* create_mesh(MeshMaterial material);
            void draw();
            ~MeshRenderer(); 
    };
}