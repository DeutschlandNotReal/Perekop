#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <pk/Pose.hpp>
#include <pkutil/SparseSet.hpp>

namespace Perekop { void draw(); }
namespace pk {
    class Mesh {
        friend void Perekop::draw();
        uint VBO{0}, EBO{0}, IBO{0};
        short flags{0};
        void r_load(); void r_reload(); void r_unload();
        public:
            static Mesh from_file(const char* path);
            struct alignas(32) Vertex {  
                glm::vec3 pos;
                glm::vec3 normal;
                glm::vec2 uv;
            };

            enum UniType {
                u_int, u_float, u_vec2, u_vec3, u_vec4, u_mat3, u_mat4
            };

            struct Uniform {
                int layout;
                UniType type;
                const void* data;
            };
            
            class Material {
                friend void Perekop::draw();
                uint program{0};
                int layout_P, layout_V;
                Array<Uniform> uniforms;
                void use(const glm::mat4& V, const glm::mat4& P) const;
                public:
                    Material() = default;
                    Material(const char* vsrc, const char* fsrc);

                    void uniform(UniType T, const char* title, const void* data);
            };
            
            short id{0};
            Material* mat{nullptr};
            Array<Vertex> vertex;
            Array<short> indices, models;

            void load();
            void reload();
            void unload();
    };

    struct Model { short id; Pose pose; glm::vec4 metadata; };

    struct Camera { float min{.1}, max{200}, fov{75}; Pose pose; };
}