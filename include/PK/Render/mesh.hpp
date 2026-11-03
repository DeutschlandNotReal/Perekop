#pragma once
#include <PK/Core/vector.hpp>
#include <PK/Render/renderer.hpp>
#include <PK/Render/Resources/material.hpp>
#include <filesystem>

namespace pk {
    extern Render::VertexArray MeshVAO;
    class Mesh;

    class Mesh {
        public:
            Render::Buffer VBO, EBO;
            struct alignas(32) Vertex { 
                vec3 pos{0.f}, nor{0.f}; vec2 uv{.0f};
            };

            Material* material{nullptr};

            vector<Vertex> vtx;
            vector<uint> ind;
            vec3 centre{0.f}, ext{0.f}, invInertia{0.f};

            void Load();
            void Unload();
            bool Loaded() const noexcept;
    
            void Refresh();

            Mesh() noexcept = default;
  
            Mesh(Mesh&&) noexcept;
            Mesh& operator=(Mesh&&) noexcept;

            Mesh(const Mesh&) noexcept;
            Mesh& operator=(const Mesh&) noexcept;

            vec3 GetEdge(uint V0, uint V1) noexcept;
            float GetArea(uint V0, uint V1, uint V2) noexcept;
            float GetVolume(uint V0, uint V1, uint V2, uint V3) noexcept;

            ~Mesh() noexcept;

            static Mesh Merge(const Mesh& A, const Mesh& B, pose poseA, pose poseB) noexcept;
    };

    class MeshImporter {
        public:
            enum class glbValue { Mesh, Material };
            struct MeshData     { Mesh* mesh; pose pose; };
            struct MaterialData {};

            using callback = std::function<void(glbValue, std::string_view, void*)>;

            // .glb format
            static void Import(const std::filesystem::path&, callback callback) noexcept;  
    };
}