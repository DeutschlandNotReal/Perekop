#pragma once
#include <PK/Graphics/mesh.hpp>
#include <PK/Physics/model.hpp>
#include <PK/Physics/body.hpp>
#include <PK/Graphics/camera.hpp>

namespace Perekop::World {   
    inline pk::vector<pk::Model> model_set;
    inline pk::vector<pk::Mesh>   mesh_set;
    inline pk::vector<pk::Body>   body_set;

    inline pk::vec3 bgcol{0.2,0.2,0.2};

    inline double fps{60};
    inline pk::Camera camera;
}