#pragma once
#include <PK/pose.hpp>
#include <PKSTL/vector.hpp>
#include <filesystem>

namespace pk {
    struct Mesh {
        unsigned VBO{0}, EBO{0}, IBO{0};

        struct alignas(32) Vertex { 
            vec3 pos{0}, nor{0};
            vec2 uv{0};
        };

        unsigned short id{0};
  
        vector<Vertex> vertices;
        vector<unsigned short> indices;

        void load();
        void unload();
        bool loaded() const noexcept;

        void refresh();
        Mesh() noexcept = default;
        Mesh(vector<Vertex>&& vertices, vector<unsigned short>&& indices) noexcept;
        Mesh(const std::filesystem::path& glb) noexcept;

        Mesh(Mesh&&) noexcept;
        Mesh& operator=(Mesh&&) noexcept;

        Mesh(const Mesh&) noexcept;
        Mesh& operator=(const Mesh&) noexcept;

        ~Mesh() noexcept;
    };
}