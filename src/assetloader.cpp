#include <PK/Render/Mesh.hpp>
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

    MeshImporter::import(world::assets/"models/zelensky.glb", [](MeshImporter::DataType type, std::string_view name, void* rawdata) {
        if (type != MeshImporter::DataType::Mesh) return;

        auto& data = *static_cast<MeshImporter::meshData*>(rawdata);
        std::printf(" loading Mesh '%*s' (vtx: %i, ind: %i)\n",
            static_cast<int>(name.size()), name.data(), data.Mesh->vtx.size(), data.Mesh->ind.size());

        auto& Mesh = MeshHandle::set.create(std::move(*data.Mesh));
        Mesh.material = &PKG::material;
        Mesh.load();

        if (name == "propeller") PKG::propeller = Mesh.getID();
        if (name == "hull") PKG::hull = Mesh.getID();

        
        Model model{.meshid = Mesh.getID(), .pose = data.pose, .scale = vec3{1.f}};
        PKG::placements.push(model);
    });


    /*
    MeshImporter::import(world::assets/"models/arrow.glb", [](MeshImporter::DataType type, std::string_view name, void* rawdata) {
        if (type != MeshImporter::DataType::Mesh) return;

        std::printf(" arrow? %*s\n", static_cast<int>(name.size()), name.data());
        auto& data = *static_cast<MeshImporter::meshData*>(rawdata);
        auto& Mesh = world::meshes.create(std::move(*data.Mesh));
        Mesh.material = &PKG::material;
        Mesh.load();
        PKG::guide = Mesh.id;
    });
    */

    printf("assetloader init finish\n");
}