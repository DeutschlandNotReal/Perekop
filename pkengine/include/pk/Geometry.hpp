#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <pk/Pose.hpp>
#include <pkutil/SparseSet.hpp>

namespace Perekop::step { void draw(); }
namespace pk {
    class Mesh {
        friend void Perekop::step::draw();
        uint VBO{0}, EBO{0}, IBO{0};
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

            class Appearance {
                friend void Perekop::step::draw();
                struct Uniform { int layout; UniType type; const void* data; };
                uint program{0}, texture{0}, layoutP{0}, layoutV{0}, layoutT{0};
                Array<Uniform> uniforms;
                void use(const glm::mat4& V, const glm::mat4& P) const;
                public:
                    Appearance() = default;
                    Appearance(const char* vpath, const char* fpath, const char* tpath = nullptr);
                    
                    void uniform(UniType T, const char* title, const void* data);
            };
            
            short id{0};
            Appearance* appearance{nullptr};
            Array<Vertex> vertex;
            Array<short> indices, models;

            void load();
            void reload();
            void unload();
    };
    struct Model { short id; Pose pose; glm::vec4 metadata; };

    struct Camera { float min{.1}, max{200}, fov{75}; Pose pose; };
}