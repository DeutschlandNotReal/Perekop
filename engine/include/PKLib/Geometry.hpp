#pragma once
#include <PKLib/math_alias.hpp>
#include <PKLib/pose.hpp>
#include <PKLib/graphics.hpp>
#include <PKCore/set.hpp>

namespace pk {
    class Model;
    class Mesh {
        friend set<Mesh>;
        friend Model;
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif
        uint32_t VBO{0}, EBO{0}, IBO{0};

        mat3 inertia{0};
        vec3 lbound, hbound;

        void load();
        void unload();

        public:
            struct alignas(32) Vertex { vec3 p, n; vec2 uv; };
            Texture texture; 
            Shader* shader{nullptr};
            
            vector<Vertex> vertices;
            vector<uint16_t> indices;

            uint16_t id;
            bool is_loaded() const { return VBO != 0; }

            Mesh() = default;
            Mesh(const Mesh&) = delete;
            Mesh& operator=(const Mesh&) = delete;
            Mesh(Mesh&&) = default;
            Mesh& operator=(Mesh&&) = default;

            ~Mesh();
    };

    class Model {
        friend set<Model>;
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif

        public:
            uint16_t id{0}, mesh{0}, body{0};
            Pose pose;
            vec4 metadata;
    };

    struct Camera { 
        float min{.01}, max{200}, fov{glm::radians(70.f)}; 
        Pose pose;

        mat4 view() const { 
            return glm::inverse(pose.mat4()); 
        }

        mat4 proj(float aspect) const { 
            return glm::perspectiveZO(fov, aspect, min, max); 
        }
    };
}