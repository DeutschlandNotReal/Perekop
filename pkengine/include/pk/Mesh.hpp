#pragma once
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#include <pk/Window.hpp>
#include <pk/Transform.hpp>
#include <pk/Model.hpp>
#include <pkutil/SparseSet.hpp>

namespace pk {
    extern const char* v_preamble;
    extern const char* f_preamble;
    class Scene;
    class Mesh {
        friend Scene;
        uint VAO{0}, VBO{0}, EBO{0}, IBO{0};
        
        public:
            struct Vertex {  
                glm::vec3 pos; glm::vec2 uv;
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
                friend Scene;
                uint program{0};
                int VP_l;
                Array<Uniform> uniforms;
                void use(const glm::mat4& VP) const;
                public:
                    Material() = default;
                    Material(const char* vsrc, const char* fsrc);

                    void add_uniform(UniType T, const char* title, const void* data);
            };
            
            short id{0};
            Material* mat{nullptr};
            Array<Vertex> vertex;
            Array<short> indices, models;

            void load();
            void unload();
            void refresh();
    };
}