
#pragma once
#include <PK/Math/pose.hpp>
#include <PK/Physics/AABB.hpp>
#include <PK/Core/vector.hpp>
#include <PK/Core/array.hpp>
#include <PK/Physics/model.hpp>
#include <PK/Physics/ray.hpp>
 
namespace pk {
    class ModelBVH {
        public:
            struct node {
                AABB bounds;
                int 
                    leaf: 1,
                    lhs:  31,
                    rhs:  31;
            };

            struct rayresult { ModelID hit; vec3 pos; float dst; };

            class iterator {
                ModelBVH* bvh;
                array<int, 50> nodestack; int nodeptr{0};

                public:
                    iterator(ModelBVH*) noexcept;
                    iterator(const ModelBVH*) noexcept;

                    auto& operator*(this auto&& v) noexcept;
                    iterator& operator++() noexcept;
                    bool operator==(bool) noexcept;
            };

            friend iterator;
        private:
            struct item { ModelID id; AABB bounds; };

            vector<item> items;
            vector<node> nodes;

        public:
            iterator begin() noexcept { return {this}; }
            const iterator begin() const noexcept { return {this}; }
            bool end() const noexcept;

            void construct(span<ModelHandle> handles);
            void clear() noexcept;
            
            const span<node> getNodes() const noexcept { return nodes; }
            span<ModelID>    query(AABB bounds) const noexcept;
            rayresult        cast(ray) const noexcept;
    };
}
 