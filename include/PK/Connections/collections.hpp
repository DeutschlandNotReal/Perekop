#pragma once
#include <PK/Render/mesh.hpp>
#include <PK/Render/model.hpp>
#include <PK/Render/Primitive/vertexattribute.hpp>

namespace pk {
    namespace Render { struct Renderer; }
    
    class Collection {
        friend struct Render::Renderer;
        void* data{nullptr}; // both models and mesh ids
        unsigned ModelCount{0}, Capacity{0};

        void Resize(unsigned);

        public:
            Render::VertexArray VAO;
            span<Model> GetModels() noexcept;
            span<const Model> GetModels() const noexcept;
            span<unsigned> GetMeshIds() noexcept;
            span<const unsigned> GetMeshIds() const noexcept;

            Model& operator[](unsigned) noexcept;
            const Model& operator[](unsigned) const noexcept;

            void AddModel(Model model, unsigned meshId);

            Collection() noexcept = default;

            Collection(Collection&&) noexcept;
            Collection& operator=(Collection&&) noexcept;

            Collection(const Collection&);
            Collection& operator=(const Collection&);

            ~Collection();
    };
}

namespace pk::World {
    inline vector<Mesh> meshes;
    inline vector<Collection> collections;
}