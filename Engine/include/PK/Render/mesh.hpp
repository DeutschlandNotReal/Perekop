#pragma once

#include <PK/Core/vector.hpp>
#include <PK/Render/Renderer.hpp>
#include <PK/Render/Resources/material.hpp>
#include <PK/Physics/AABB.hpp>
#include <filesystem>
#include <PK/Core/set.hpp>

namespace pk {
    inline VertexArr meshVAO;

    class Mesh {
        public:
            struct Vertex { 
                vec3 pos{0.f}, nor{0.f}; vec2 uv{.0f};
            };

            buffer VBO, EBO;
            Material* material{nullptr};

            vector<Vertex> vtx;
            vector<uint> ind;
            AABB box;
            vec3 invInertia{0.f};

            void load();
            void unload();
            bool loaded() const noexcept;
    
            void refresh();

            Mesh() noexcept = default;
  
            Mesh(Mesh&&) noexcept;
            Mesh& operator=(Mesh&&) noexcept;

            Mesh(const Mesh&) noexcept;
            Mesh& operator=(const Mesh&) noexcept;

            vec3 getEdge(uint V0, uint V1) noexcept;
            float getArea(uint V0, uint V1, uint V2) noexcept;
            float getVolume(uint V0, uint V1, uint V2, uint V3) noexcept;

            ~Mesh() noexcept;
    };

    using MeshHandle = handle<Mesh>;
 
    class MeshImporter {
        public:
            enum class DataType { Mesh, material };
            struct meshData     { Mesh* Mesh; pose pose; };
            struct materialData {};

            using callback = std::function<void(DataType, std::string_view, void*)>;

            // .glb format
            static void import(const std::filesystem::path&, callback callback) noexcept;
    };
}