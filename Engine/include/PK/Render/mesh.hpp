#pragma once

#include <PK/Core/vector.hpp>
#include <PK/Render/renderer.hpp>
#include <PK/Render/Resources/material.hpp>
#include <PK/Physics/AABB.hpp>

#include <filesystem>

namespace pk {
    inline vtxAttribute meshVAO;

    class mesh {
        public:
            struct vertex { 
                vec3 pos{0.f}, nor{0.f}; vec2 uv{.0f};
            };

            buffer VBO, EBO;
            Material* material{nullptr};

            vector<vertex> vtx;
            vector<uint> ind;
            AABB box;
            vec3 invInertia{0.f};

            void load();
            void unload();
            bool loaded() const noexcept;
    
            void refresh();

            mesh() noexcept = default;
  
            mesh(mesh&&) noexcept;
            mesh& operator=(mesh&&) noexcept;

            mesh(const mesh&) noexcept;
            mesh& operator=(const mesh&) noexcept;

            vec3 getEdge(uint V0, uint V1) noexcept;
            float getArea(uint V0, uint V1, uint V2) noexcept;
            float getVolume(uint V0, uint V1, uint V2, uint V3) noexcept;

            ~mesh() noexcept;
    };

    class meshImporter {
        public:
            enum class glbValue { mesh, material };
            struct meshData     { mesh* mesh; pose pose; };
            struct materialData {};

            using callback = std::function<void(glbValue, std::string_view, void*)>;

            // .glb format
            static void import(const std::filesystem::path&, callback callback) noexcept;
    };
}