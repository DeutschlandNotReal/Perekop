#pragma once
#include <PK/Graphics/mesh.hpp>
#include <PK/Physics/model.hpp>
#include <PK/Physics/body.hpp>
#include <PK/Graphics/camera.hpp>

namespace Perekop::World {   
    inline pk::vector<pk::Model> models;
    inline pk::vector<pk::Mesh>  meshes;
    inline pk::vector<pk::Body>  bodies;

    inline pk::vec3 bgcol{0.2,0.2,0.2};

    inline f64 fps{60};
    inline pk::Camera camera;
}