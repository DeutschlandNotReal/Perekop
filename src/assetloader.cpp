#include <PK/Render/mesh.hpp>
#include <PK/tick.hpp>
#include <PK/world.hpp>

#include <cstdio>
#include <string_view>
#include <utility>

#include "./common.hpp"
#include "./functions.hpp"

using namespace pk;


void PKG::onInit::assets() {
    printf("assetloader init\n");

    meshImporter::import(world::assets/"models/zelensky.glb", [](meshImporter::glbValue type, std::string_view name, void* rawdata) {
        if (type != meshImporter::glbValue::mesh) return;

        auto& data = *static_cast<meshImporter::meshData*>(rawdata);
        std::printf(" loading mesh '%*s' (vtx: %i, ind: %i)\n",
            static_cast<int>(name.size()), name.data(), data.mesh->vtx.size(), data.mesh->ind.size());

        auto& mesh = world::meshes.create(std::move(*data.mesh));
        mesh.material = &PKG::material;
        mesh.load();

        if (name == "propeller") PKG::propeller = mesh.id;
        if (name == "hull") PKG::hull = mesh.id;

        model model{.meshid = mesh.id, .pose = data.pose, .scale = vec3{1.f}};
        PKG::placements.push(model);
    });


    /*
    meshImporter::import(world::assets/"models/arrow.glb", [](meshImporter::glbValue type, std::string_view name, void* rawdata) {
        if (type != meshImporter::glbValue::mesh) return;

        std::printf(" arrow? %*s\n", static_cast<int>(name.size()), name.data());
        auto& data = *static_cast<meshImporter::meshData*>(rawdata);
        auto& mesh = world::meshes.create(std::move(*data.mesh));
        mesh.material = &PKG::material;
        mesh.load();
        PKG::guide = mesh.id;
    });
    */

    printf("assetloader init finish\n");
}