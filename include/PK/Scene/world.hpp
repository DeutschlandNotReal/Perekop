#pragma once
#include <PK/Render/mesh.hpp>
#include <PK/Physics/model.hpp>
#include <PK/Physics/body.hpp>
#include <PK/Physics/joint.hpp>
#include <PK/Render/Resources/vertexattribute.hpp>

#include <PK/Core/set.hpp>


namespace pk::World {
    inline set<Mesh>  meshes;
    inline set<Model> models;
    inline set<Physics::Body> bodies;
    inline set<Physics::Joint> joints;
}