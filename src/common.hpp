#pragma once
#include <PK/Render/Mesh.hpp>
#include <PK/Physics/Model.hpp>

namespace PKG {
    inline pk::id<pk::MeshHandle> propeller, hull;
    inline pk::Material material;

    inline pk::vector<pk::Model> placements;
}