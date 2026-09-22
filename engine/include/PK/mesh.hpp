#pragma once
#include <PK/pose.hpp>
#include <PKSTL/vector.hpp>
#include <filesystem>

namespace pk {
    struct Texture;
    struct Shader;
    
    using path = std::filesystem::path;

    class Render;
    class Mesh {
        friend Render;
        unsigned VBO{0}, EBO{0}, IBO{0};

        public:
            struct alignas(32) Vertex { 
                vec3 pos{0}, nor{0};
                vec2 uv{0};
            };

            Texture* texture{nullptr};
            Shader* shader{nullptr};

            vector<Vertex> vertices;
            vector<unsigned> indices;

            void Load();
            void Unload();
            bool Loaded() const noexcept;

            void Refresh();

            Mesh() noexcept = default;
            Mesh(const path& glb) noexcept;

            Mesh(Mesh&&) noexcept;
            Mesh& operator=(Mesh&&) noexcept;

            Mesh(const Mesh&) noexcept;
            Mesh& operator=(const Mesh&) noexcept;

            ~Mesh() noexcept;   
    };
}