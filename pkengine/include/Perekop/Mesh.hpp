#pragma once
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#include <Perekop/Array.hpp>
#include <Perekop/StableArray.hpp>
#include <Perekop/Window.hpp>
#include <Perekop/Transform.hpp>

using ID_T = unsigned short;

namespace pk {
    class Mesh;
    struct Model {
        Transform transform;
        glm::vec3 scl{1};
        Mesh* mesh;
        short userid;
        Model(Mesh* mesh);
        ~Model();
    };

    struct Camera {
        float f = 200.f, n = .1f, fov = 75.f;
        Transform transform;

        glm::mat4 VP(glm::vec2 screen_size) const noexcept;
    };

    struct MeshVertex { 
        glm::vec3 pos; glm::vec2 uv;
        MeshVertex(): pos(0), uv(0) {}
        MeshVertex(float x, float y, float z): pos(x, y, z), uv(0) {};
        MeshVertex(float x, float y, float z, float u, float v): pos(x, y, z), uv(u, v) {}
        MeshVertex(glm::vec3 p): pos(p), uv(0) {}
        MeshVertex(glm::vec3 p, glm::vec2 UV): pos(p), uv(UV) {}
    };

    struct MeshTriangle { 
        ID_T v0, v1, v2; 
        MeshTriangle(ID_T A, ID_T B, ID_T C): v0(A), v1(B), v2(C) {}
    };

    class Scene;
    class MeshMaterial {
        friend Scene;
        unsigned int program{0};
        void enable(const glm::mat4& VP);
        public:
            MeshMaterial() = default;
            MeshMaterial(const char* vertex_source, const char* fragment_source);
    };

    class Mesh {
        friend Scene;
        unsigned int VAO{0}, VBO{0}, EBO{0}, IBO{0};
        MeshMaterial mat;
        public:
            Array<MeshVertex> vertex;
            Array<MeshTriangle> triangle;
            Array<Model*> users;

            void bounds(glm::vec3& min, glm::vec3& max) const noexcept;
            void load();
            void unload();
            void refresh();
            Mesh(MeshMaterial material): mat(material) {}
            ~Mesh();
    };

    class Scene {
        StableArray<Mesh*, short> meshes;
        Array<glm::mat3x4> transforms{50};

        public:
            template <typename... A> short create_mesh();
            Mesh& get_mesh(short id) const noexcept { return *meshes[id]; } 
            void draw(const Window& win);
    };
}