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
    MeshImporter::import(world::assets/"models/zelensky.glb", [](MeshImporter::Data info) {
        if (info.type != MeshImporter::Data::MeshType) return;

        Mesh& datamesh = *info.meshdata.ptr;
        pose pose = info.meshdata.pose;

        MeshHandle& mesh = MeshHandle::create(std::move(datamesh));
        pk::success(std::format("Imported mesh '{}' (i: {}, v: {}, uid: {})", info.name, mesh.ind.size(), mesh.vtx.size(), (uint) mesh.getID()));

        mesh.material = &PKG::material;
        mesh.load();

        if (info.name == "propeller") PKG::propeller = mesh.getID();
        if (info.name == "hull") PKG::hull = mesh.getID();
        
        Model model{.meshID = mesh.getID(), .pose = pose, .scale = vec3{1.f}, .bounds = mesh.box};

        PKG::placements.push(std::move(model));
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
}