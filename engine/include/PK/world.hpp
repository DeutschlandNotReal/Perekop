#pragma once
#include <PK/mesh.hpp>
#include <PK/model.hpp>
#include <PK/body.hpp>
#include <PK/camera.hpp>
#include <PKSTL/set.hpp>
#include <filesystem>

namespace Perekop::World {
    inline vec3 bgcol{0.2,0.2,0.2};

    inline double fps{60};
    inline pk::Camera camera;
}