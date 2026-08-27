#pragma once
#include <PK/pose.hpp>
#include <PK/texture.hpp>
#include <PK/shader.hpp>

namespace pk {
    class Model;

    class Mesh {
        friend Model;
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif

        unsigned VBO{0}, EBO{0}, IBO{0};

        void load();
        void unload();

        public:
            struct alignas(32) Vertex { 
                vec3 p{0.f}, n{0.f}; vec2 uv{0.f};
            };

            unsigned short id{0};
  
            vector<Vertex> vertices;
            vector<unsigned short> indices;

            [[nodiscard]] bool loaded() const noexcept { return VBO != 0; }

            void refresh() { if (loaded()) load();  }
            Mesh() noexcept = default;
            Mesh(Mesh&& b) noexcept: vertices(std::move(b.vertices)), indices(std::move(b.indices)) {
                VBO = b.VBO; EBO = b.EBO; IBO = b.IBO;
                b.EBO = b.IBO = b.VBO = 0;
            }

            Mesh(decltype(Mesh::vertices)&& vertices, decltype(Mesh::indices)&& indices) noexcept: 
                vertices(std::move(vertices)), indices(std::move(indices))
            {}

            Shader* shader{nullptr}; 
            Texture* texture{nullptr};

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