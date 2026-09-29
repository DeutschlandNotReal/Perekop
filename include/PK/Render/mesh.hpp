#pragma once
#include <PK/Container/vector.hpp>
#include <PK/Render/renderer.hpp>
#include <PK/Render/Primitive/material.hpp>
#include <filesystem>

namespace pk {
    extern Render::VertexArray MeshVAO;

    class Mesh {
        public:
            Render::Buffer VBO, EBO;
            bool loaded{false};

            struct alignas(32) Vertex { 
                vec3 pos{0}, nor{0};
                vec2 uv{0};
            };

            Material* material{nullptr};

            vector<Vertex> vertices;
            vector<unsigned> indices;

            void Load();
            void Unload();
            bool Loaded() const noexcept;
            static void Initialize() noexcept;

            void Refresh();

            Mesh() noexcept = default;
            Mesh(const std::filesystem::path& glb) noexcept;

            Mesh(Mesh&&) noexcept;
            Mesh& operator=(Mesh&&) noexcept;

            Mesh(const Mesh&) noexcept;
            Mesh& operator=(const Mesh&) noexcept;

            ~Mesh() noexcept;
    };
}