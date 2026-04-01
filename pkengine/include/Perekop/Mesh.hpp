#pragma once
#include <glm/glm.hpp>

using ID_T = unsigned short;

// shorthand but also makes resize function for each
#define PK_ARRAY(T, N) T* N = new T[1]; ID_T N##_count = 0; ID_T N##_capacity = 1; void resize_##N(ID_T size) { if (!size) return; N##_capacity = size; T* prev = N; N = new T[size]; for (ID_T i = 0; i < N##_count; i++) N[i] = prev[i]; delete[] prev; };

namespace pk {
    class Model;
    class MeshRenderer;
    class Camera;
    class Mesh;

    struct MeshVertex { 
        glm::vec3 pos; glm::vec2 uv;
        MeshVertex(): pos(0), uv(0) {}
        MeshVertex(float x, float y, float z): pos(x, y, z), uv(0) {};
        MeshVertex(float x, float y, float z, float u, float v): pos(x, y, z), uv(u, v) {}
        MeshVertex(glm::vec3 p): pos(p), uv(0) {}
    };

    struct MeshTriangle { ID_T v0, v1, v2; };


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

        private:
            unsigned int VAO = 0, VBO = 0, EBO = 0, IBO = 0;
            ID_T meshid = 0;
            MeshRenderer* renderer;
            
            PK_ARRAY(MeshVertex, vertex)
            PK_ARRAY(MeshTriangle, triangle)
            PK_ARRAY(Model*, users)

            ~Mesh();
        public:
            Mesh(MeshMaterial mat): material(mat) {}
            MeshMaterial material;
            ID_T push_vertex(MeshVertex vertex);
            ID_T pop_vertex();
            void set_vertex(ID_T vid, MeshVertex new_vertex);

            ID_T push_vertex(float x, float y, float z) { return push_vertex(MeshVertex{x, y, z}); }
            ID_T push_vertex(float x, float y, float z, float u, float v) { return push_vertex(MeshVertex{x, y, z, u, v}); }

            ID_T push_triangle(MeshTriangle triangle);
            ID_T pop_triangle();
            void set_triangle(ID_T vid, MeshTriangle new_triangle);

            ID_T push_triangle(ID_T v0, ID_T v1, ID_T v2) { return push_triangle(MeshTriangle{v0, v1, v2}); } 

            void load();
            void refresh();
            void unload();
    };

    class MeshRenderer {
        friend Mesh;
        private:
            PK_ARRAY(Mesh*, meshes)
            PK_ARRAY(glm::mat3x4, transforms)
        public:
            Mesh* create_mesh(MeshMaterial material);
            void draw();    
            ~MeshRenderer(); 
    };
}

#undef PK_ARRAY