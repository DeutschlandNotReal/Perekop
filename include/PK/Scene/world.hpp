#pragma once
#include <PK/Render/mesh.hpp>
#include <PK/Physics/model.hpp>
#include <PK/Physics/body.hpp>
#include <PK/Render/Resources/vertexattribute.hpp>


namespace pk::World {
    inline vector<Mesh>  meshes;
    inline vector<Model> models;
    inline vector<Physics::Body> bodies;
}