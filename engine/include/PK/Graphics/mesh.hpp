#pragma once
#include <PK/pch.hpp>

#include <PK/Graphics/texture.hpp>
#include <PK/Graphics/shader.hpp>

namespace pk {
    class Model;

    class Mesh {
        friend Model;
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif

        u32 VBO{0}, EBO{0}, IBO{0};

        void load();
        void unload();

        public:
            struct alignas(32) Vertex { vec3 p, n; vec2 uv; };
  
            vector<Vertex> vertices;
            vector<u16> indices;

            [[nodiscard]] bool loaded() const noexcept { return VBO != 0; }

            Mesh() = default;
            Mesh(Mesh&& b) noexcept: vertices(std::move(b.vertices)), indices(std::move(b.indices)) {
                VBO = b.VBO; EBO = b.EBO; IBO = b.IBO;
                b.EBO = b.IBO = b.VBO = 0;
            }

            Shader* shader; Texture* texture; // naive pointer may maybe change later maybe

            Mesh& operator=(Mesh&& b) noexcept {
                if (this == &b) return *this;
                if (loaded()) unload();

                vertices = std::move(b.vertices);
                indices = std::move(b.indices);
                VBO = b.VBO; EBO = b.EBO; IBO = b.IBO;
                b.EBO = b.IBO = b.VBO = 0;

                return *this;
            }

            Mesh(const Mesh& b) noexcept: vertices(b.vertices), indices(b.indices) {};

            Mesh& operator=(const Mesh& b) noexcept {
                if (this == &b) return *this;
                if (loaded()) unload();
                EBO = IBO = VBO = 0;

                vertices = b.vertices;
                indices = b.indices;

                return *this;
            }

            ~Mesh() noexcept { unload(); };
    };
}