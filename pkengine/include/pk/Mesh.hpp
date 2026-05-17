#pragma once
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#include <pk/Window.hpp>
#include <pk/Transform.hpp>
#include <pkutil/Pool.hpp>

namespace pk {
    class Mesh;
    struct Model {
        uint id;
        glm::vec3 scale{1};
        Transform transform;
        glm::mat3x4 get_scaled() const noexcept {
            return {
                {transform.rot[0] * scale.x, transform.pos.x},
                {transform.rot[1] * scale.y, transform.pos.y},
                {transform.rot[2] * scale.z, transform.pos.z},
            };
        }
    };

    struct Camera {
        float f = 200.f, n = .1f, fov = 75.f;
        Transform transform;
        glm::mat4 VP(glm::vec2 screen_size) const noexcept;
    };

    struct MeshVertex { 
        glm::vec3 pos; 
        glm::vec2 uv;
    };

    class Scene;
    class MeshMaterial {
        friend Scene;
        unsigned int program{0};
        void enable(const glm::mat4& VP) const;
        public:
            MeshMaterial() = default;
            MeshMaterial(const char* vertex_source, const char* fragment_source);
    };

    class Mesh {
        friend Scene;
        uint VAO{0}, VBO{0}, EBO{0}, IBO{0};
        public:
            uint id{0};
            MeshMaterial mat;
            pkutil::Array<MeshVertex> vertex;
            pkutil::Array<short> indices;
            pkutil::Array<short> models;

            void bounds(glm::vec3& min, glm::vec3& max) const noexcept;
            void load();
            void unload();
            void refresh();
            void rewind() noexcept;
    };

    class Scene {
        pkutil::StablePool<Mesh> meshes;
        pkutil::StablePool<Model> models;
        pkutil::Array<glm::mat3x4> transforms{50};

        public:
            Mesh& create_mesh() { 
                uint id = meshes.emplace();
                meshes[id].id = id; 
                return meshes[id];
            }
            Model& create_model() { 
                uint id = models.emplace();
                models[id].id = id;
                return models[id];
            }
            void link(uint meshid, uint modelid) {
                meshes[meshid].models.push(modelid);
            }
            void remove(Mesh& mesh) {
                meshes.remove(mesh.id);
            }
            void remove(Model& model) {
                models.remove(model.id);
            }

            void draw(const Window& win);
    };
}