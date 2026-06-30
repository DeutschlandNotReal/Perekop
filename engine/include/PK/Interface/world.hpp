#pragma once
#include <PK/Core/set.hpp>
#include <PK/Graphics/mesh.hpp>
#include <PK/Physics/model.hpp>
#include <PK/Physics/body.hpp>
#include <PK/Graphics/camera.hpp>

namespace Perekop::World {   
    inline pk::set<pk::Model> models;
    inline pk::set<pk::Mesh> meshes;
    inline pk::set<pk::Body> bodies;
    inline glm::vec3 bgcol{0.2,0.2,0.2};

    inline double fps{60};
    inline pk::Camera camera;
}