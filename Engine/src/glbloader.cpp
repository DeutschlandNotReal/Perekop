#include <PK/Render/Mesh.hpp>

#include <cgltf.h>

using namespace pk;

namespace {
void LoadMesh(cgltf_mesh &gmesh, pose pose, MeshImporter::callback callback, vec3 scale) noexcept {
    for (uint pid{0}; pid < gmesh.primitives_count; pid++) {
        Mesh Mesh;
        auto& vtx = Mesh.vtx;
        auto& ind = Mesh.ind;

        cgltf_accessor *pos{nullptr}, *nor{nullptr}, *uv{nullptr};
        const auto& primitive = gmesh.primitives[pid];

        for (uint attid{0}; attid < primitive.attributes_count; attid++) {
            const auto& attribute = primitive.attributes[attid];
                
            switch (attribute.type) {
                case cgltf_attribute_type_position: pos = attribute.data; break;
                case cgltf_attribute_type_normal:   nor = attribute.data; break;
                case cgltf_attribute_type_texcoord: uv  = attribute.data; break;
                default: break;
            }
        }

        if (!pos || !pos->count) continue;

        vec3 centre{0.f};
        vtx.reserve(vtx.size() + pos->count);
        for (uint vid{0}; vid < pos->count; vid++) {
            Mesh::Vertex& Vertex = vtx.emplace();
            cgltf_accessor_read_float(pos, vid, &Vertex.pos.x, 3);

            centre += Vertex.pos *= scale;
            if (nor) cgltf_accessor_read_float(nor, vid, &Vertex.nor.x, 3);
            if (uv)  cgltf_accessor_read_float(uv, vid, &Vertex.uv.x, 2);
        } 
        centre /= (float) vtx.size();

        if (primitive.indices) {
            uint vid{};
            ind.reserve(ind.size() + primitive.indices->count);
            for (uint iid{0}; iid < primitive.indices->count; iid++) {
                cgltf_accessor_read_uint(primitive.indices, iid, &vid, 1);
                ind.push(vid);
            }
        } else {
            ind.reserve(vtx.size());
            for (uint vid{0}; vid < vtx.size(); vid++) ind.push(vid);
        }

        for (Mesh::Vertex &v : vtx) v.pos -= centre;
        MeshImporter::meshData data{.Mesh = &Mesh, .pose = pose + worldspace(centre, pose.rot)};

        callback(MeshImporter::DataType::Mesh, gmesh.name, &data);
    }
}

void ProcessNode(cgltf_node* node, pose pose, vec3 scale, MeshImporter::callback callback) noexcept {
    if (node->has_matrix)
        pose *= *(mat4*)node->matrix;

    if (node->has_rotation) 
        pose *= *(quat*)node->rotation;

    if (node->has_translation) 
        pose += *(vec3*)node->translation;

    if (node->has_scale) 
        scale *= *(vec3*)node->scale;

    pose.rot = normalize(pose.rot);

    if (node->mesh) 
        LoadMesh(*node->mesh, pose, callback, scale);

    for (uint child{0}; child < node->children_count; child++) 
        ProcessNode(node->children[child], pose, scale, callback);
}

}

void MeshImporter::import(const std::filesystem::path& path, MeshImporter::callback callback) noexcept {
    cgltf_options options{};
    cgltf_data* data{nullptr};
    auto spath = path.string();

    if (cgltf_parse_file(&options, spath.c_str(), &data) != cgltf_result_success)
        return;

    if (cgltf_load_buffers(&options, data, spath.c_str()) != cgltf_result_success) {
        cgltf_free(data);
        return;
    }

    printf("Importing %S\n", path.c_str());

    for (cgltf_size sceneid{0}; sceneid < data->scenes_count; sceneid++) {
        cgltf_scene& scene = data->scenes[sceneid];
        

        for (cgltf_size nid{0}; nid < scene.nodes_count; nid++)
            ProcessNode(scene.nodes[nid], pose{}, vec3{1.f}, callback);
    }
    
    cgltf_free(data);
}