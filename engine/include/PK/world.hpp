#pragma once
#include <PK/mesh.hpp>
#include <PK/model.hpp>
#include <PK/body.hpp>
#include <PK/camera.hpp>
#include <PKSTL/set.hpp>

namespace Perekop::World { 
    inline pk::set<pk::Model> models{}; 
    inline pk::set<pk::Mesh>  meshes{};
    inline pk::set<pk::Body>  bodies{};

    inline vec3 bgcol{0.2,0.2,0.2};

    inline double fps{60};
    inline pk::Camera camera;

    // .glb / .gltf format
    void import_scene(
        const std::filesystem::path& path,
        pk::vector<decltype(models)::handle>* models = nullptr, 
        pk::vector<decltype(meshes)::handle>* meshes = nullptr
    );
}