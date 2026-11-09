#pragma once
#include <PK/Render/mesh.hpp>
#include <PK/Core/setID.hpp>
#include <PK/Physics/model.hpp>

namespace PKG {
    inline pk::setID propeller, hull, guide;
    inline pk::Material material;

    inline pk::vector<pk::model> placements;
}