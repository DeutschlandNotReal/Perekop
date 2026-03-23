#pragma once
#include "glm/glm.hpp"
#include <vector>
#include <functional>
// really meshmodelmaterial.hpp but that sounds stupid

using uint16 = unsigned short;

namespace pk {
    class Mesh;
    class Model;
    struct MeshMaterial;
    
    struct MeshVertex { 
        glm::vec3 pos; glm::vec2 uv;
        MeshVertex(glm::vec3 POS, glm::vec2 UV): pos(POS), uv(UV) {} 
    };
    struct MeshTrig { 
        uint16 v0, v1, v2; 
        MeshTrig(uint16 V0, uint16 V1, uint16 V2): v0(V0), v1(V1), v2(V2) {};
    };

    class MeshRenderer {
        friend Mesh;
        private:
            std::vector<Mesh*> meshes;
        public:
            int draw();
            int draw_tested(std::function<bool(Mesh&, Model&)>);

            Mesh* create_mesh();
            MeshMaterial create_material(const char* vsrc, const char* fsrc);
    };

    struct MeshMaterial {
        friend MeshRenderer;
        unsigned int program;
        public: MeshMaterial() = delete;
        private: MeshMaterial(unsigned int prog);
    };

    class Mesh {
        friend Model;
        friend MeshRenderer;
        private:
            Mesh(uint16 i);
            uint16 index;
            std::vector<MeshVertex> verts;
            std::vector<MeshTrig> trigs;
            unsigned int VAO, VBO, EBO, IBO;
            std::vector<Model*> users;
            MeshMaterial material;
        public:
            Mesh() = delete;
            ~Mesh();
            uint16 pop_triangle();
            uint16 push_triangle(uint16 v0, uint16 v1, uint16 v2);
            uint16 pop_vertex();
            uint16 push_vertex(glm::vec3 pos, glm::vec2 uv = glm::vec2(0, 0));
            void set_position(uint16 vid, glm::vec3 pos);
            void set_uv(uint16 vid, glm::vec2 uv);
            void set_verticies(uint16 tid, uint16 v0, uint16 v1, uint16 v2);

            void clear_geometry();

            void flush();

            void disassign_model(Model* model);
            void assign_model(Model* model);
    };

    class Model {
        friend Mesh;
        friend MeshRenderer;
        private:
            uint16 index;
            Mesh* mesh;
        public:
            glm::mat3x4 matrix;
            glm::vec3 size;
            bool visible;
            ~Model();
            void set_mesh(Mesh* new_mesh);
            inline Mesh* get_mesh();
    };
}