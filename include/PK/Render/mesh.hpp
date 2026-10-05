#pragma once
#include <PK/Core/vector.hpp>
#include <PK/Render/renderer.hpp>
#include <PK/Render/Resources/material.hpp>
#include <filesystem>

namespace pk {
    extern Render::VertexArray MeshVAO;

    class Mesh {
        public:
            Render::Buffer VBO, EBO;
            struct alignas(32) Vertex { 
                vec3 pos{0.f}, nor{0.f}; vec2 uv{.0f};
            };

            struct Triangle {
                unsigned V0{0}, V1{0}, V2{0};
            };

            Material* material{nullptr};

            vector<Vertex> v; 
            vector<Triangle> t;

            void Load();
            void Unload();
            bool Loaded() const noexcept;
            //static void Initialize() noexcept;

            void Refresh();

            Mesh() noexcept = default;
            Mesh(const std::filesystem::path& glb) noexcept;

            Mesh(Mesh&&) noexcept;
            Mesh& operator=(Mesh&&) noexcept;

            Mesh(const Mesh&) noexcept;
            Mesh& operator=(const Mesh&) noexcept;

            vec3 GetEdge(unsigned V0, unsigned V1) noexcept;
            float GetArea(unsigned V0, unsigned V1, unsigned V2) noexcept;
            float GetVolume(unsigned V0, unsigned V1, unsigned V2, unsigned V3) noexcept;

            ~Mesh() noexcept;
    };
}