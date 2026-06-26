#pragma once
#include <PKCore/slot_map.hpp>
#include <PKAlias/math.hpp>
#include <PKLib/pose.hpp>
#include <PKLib/texture.hpp>
#include <PKLib/shader.hpp>

namespace pk {
    class Model;
    class Mesh {
        friend slot_map<Mesh>;
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
            vector<u16> indices;

            u16 id;
            bool is_loaded() const { return VBO != 0; }

            Mesh() = default;
            Mesh(const Mesh&) = delete;
            Mesh& operator=(const Mesh&) = delete;
            Mesh(Mesh&&) = default;
            Mesh& operator=(Mesh&&) = default;

            ~Mesh();
    };
}