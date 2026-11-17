#include "./loader.hpp"
#include <PK/world.hpp>

using namespace pk;

void PKG::ImportPlacement(      
    const std::filesystem::path& path, 
    pk::vector<pk::Model>& out,
    std::function<void(pk::MeshHandle&, std::string_view)> namer
) {
     MeshImporter::import(path, [&](MeshImporter::Data info) {
        if (info.type != MeshImporter::Data::MeshType) return;

        Mesh& datamesh = *info.meshdata.ptr;
        pose pose = info.meshdata.pose;

        MeshHandle& mesh = MeshHandle::create(std::move(datamesh));
        namer(mesh, info.name);
        
        Model model{.meshID = mesh.getID(), .pose = pose, .scale = vec3{1.f}, .bounds = mesh.bounds};

        out.push(std::move(model));
    });
}
