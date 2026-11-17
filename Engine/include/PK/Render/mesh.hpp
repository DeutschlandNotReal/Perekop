#pragma once

#include <PK/Core/vector.hpp>
#include <PK/Physics/AABB.hpp>
#include <filesystem>
#include <PK/Core/set.hpp>

#include <PK/Render/Resources/vertexarr.hpp>
#include <PK/Render/Resources/buffer.hpp>

namespace pk {
    class Material;

    class Mesh {
        public:
            struct Vertex { 
                vec3 pos{0.f}, nor{0.f}; vec2 uv{.0f};
            };

            static inline VertexArr VAO;
            Buffer VBO, EBO;
            Material* material{nullptr};

            vector<Vertex> vtx;
            vector<uint> ind;
            int tag{0};
            AABB bounds;
            vec3 inertia{0.f};

            void load();
            void unload();
            bool loaded() const noexcept;
    
            void synctraits() noexcept;

            Mesh() noexcept = default;
  
            Mesh(Mesh&&) noexcept;
            Mesh& operator=(Mesh&&) noexcept;
    };

    using MeshHandle = handle<Mesh>;
    using MeshID     = SID<Mesh>;
 
    class MeshImporter {
        public:
            struct Data {
                enum Type { MeshType, MatType };
                std::string_view name;
                Type type;
                union {
                    struct { Mesh* ptr; pose pose; } meshdata;
                };
            };

            using callback = std::function<void(Data)>;

            // .glb format
            static void import(const std::filesystem::path&, callback callback) noexcept;
    };
}