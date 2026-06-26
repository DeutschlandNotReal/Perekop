#pragma once
#include <PKCore/slot_map.hpp>
#include <PKLib/mesh.hpp>
#include <PKLib/model.hpp>
#include <PKLib/body.hpp>
#include <PKLib/camera.hpp>

namespace Perekop::World {   
    inline pk::slot_map<pk::Model> models;
    inline pk::slot_map<pk::Mesh> meshes;
    inline pk::slot_map<pk::Body> bodies;
    inline glm::vec3 bgcol{0.2,0.2,0.2};

    inline double fps{50};
    inline pk::Camera camera;
}