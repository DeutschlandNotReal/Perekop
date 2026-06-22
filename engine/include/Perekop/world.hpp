#pragma once
#include <PKCore/set.hpp>
#include <PKLib/mesh.hpp>
#include <PKLib/model.hpp>
#include <PKLib/body.hpp>
#include <PKLib/camera.hpp>

namespace Perekop::World {   
    inline pk::set<pk::Model> models;
    inline pk::set<pk::Mesh> meshes;
    inline pk::set<pk::Body> bodies;
    inline glm::vec3 bgcol{0.2,0.2,0.2};

    inline double fps{50};
    inline pk::Camera camera;
}