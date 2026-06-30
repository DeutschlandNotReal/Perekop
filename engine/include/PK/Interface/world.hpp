#pragma once
#include <PK/Core/set.hpp>
#include <PK/Graphics/mesh.hpp>
#include <PK/Physics/model.hpp>
#include <PK/Physics/body.hpp>
#include <PK/Graphics/camera.hpp>

namespace Perekop::World {   
    inline pk::Vec<pk::Model> model_set;
    inline pk::Vec<pk::Mesh>   mesh_set;
    inline pk::Vec<pk::Body>   body_set;

    inline glm::vec3 bgcol{0.2,0.2,0.2};

    inline double fps{60};
    inline pk::Camera camera;
}