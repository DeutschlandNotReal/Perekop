#pragma once
#include <PK/Render/mesh.hpp>
#include <PK/Physics/model.hpp>
#include <PK/Physics/body.hpp>
#include <PK/Physics/collision.hpp>
#include <PK/Physics/joint.hpp>
#include <PK/Render/Resources/vertexattribute.hpp>
#include <PK/Render/Resources/material.hpp>
#include <PK/Core/set.hpp>

#include <filesystem>

namespace pk::world {
    inline set<mesh>  meshes;
    inline set<model> models;
    inline set<body>  bodies;
    inline set<physics::joint> joints;

    inline std::filesystem::path assets{"assets"};
}