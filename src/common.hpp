#pragma once
#include <PK/Render/Mesh.hpp>
#include <PK/Physics/Model.hpp>
#include <PK/Render/Resources/material.hpp>

namespace PKG {
    inline pk::MeshID propeller, hull;
    inline pk::Material material;

    inline pk::vector<pk::Model> placements;
}