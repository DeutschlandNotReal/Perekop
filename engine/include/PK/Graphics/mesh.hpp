#pragma once
#include <PK/Core/set.hpp>
#include <PK/Physics/pose.hpp>
#include <PK/Graphics/texture.hpp>
#include <PK/Graphics/shader.hpp>

namespace pk {
    class Model;
    class Mesh {
        friend set<Mesh>;
        friend Model;
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif
        u32 VBO{0}, EBO{0}, IBO{0};

        void load();
        void unload();

        public:
            struct alignas(32) Vertex { vec3 p, n; vec2 uv; };
            Texture texture; 
            Shader* shader{nullptr};
            
            Vec<Vertex> vertices;
            Vec<u16> indices;

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