#define PK_INTERNAL
#include <PKINT/internal.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <cgltf.h>
#include <PK/file.hpp>
#include <utility>
#include <PK/mesh.hpp>

using namespace pk;
using std::string_view;
using std::string;

bool Mesh::loaded() const noexcept { return VBO != 0; }
void Mesh::refresh() { if (loaded()) { unload(); load(); }}

Mesh::Mesh(Mesh&& b) noexcept: 
    VBO(std::exchange(b.VBO, 0)),
    EBO(std::exchange(b.EBO, 0)),
    IBO(std::exchange(b.IBO, 0)),
    vertices(std::move(b.vertices)),
    indices(std::move(b.indices))
{}

Mesh& Mesh::operator=(Mesh&& b) noexcept {
    if (this == &b) return *this;
    if (loaded()) unload();

    vertices = std::move(b.vertices);
    indices = std::move(b.indices);
    VBO = std::exchange(b.VBO, 0);
    EBO = std::exchange(b.EBO, 0);
    IBO = std::exchange(b.IBO, 0);

    return *this;
}

Mesh& Mesh::operator=(const Mesh& b) noexcept {
    if (this == &b) return *this;
    if (loaded()) unload();
    EBO = IBO = VBO = 0;

    vertices = b.vertices;
    indices = b.indices;
  
    return *this;
}

Mesh::Mesh(vector<Vertex>&& vertices, vector<unsigned short>&& indices) noexcept:
    vertices(std::move(vertices)),
    indices(std::move(indices)) 
{}


Mesh::Mesh(const Mesh& b) noexcept: 
    vertices{b.vertices},
    indices{b.indices}
{}

Mesh::~Mesh() noexcept { unload(); }

Mesh::Mesh(const std::filesystem::path& path) noexcept {
    cgltf_options options{};  
    cgltf_data* data{nullptr};
    auto spath = path.string();

    cgltf_result result = cgltf_parse_file(&options, spath.c_str(), &data);

    result = cgltf_load_buffers(&options, data, spath.c_str());

    for (unsigned id = 0; id < data->meshes_count; id++) {
        cgltf_mesh& gmesh = data->meshes[id];
        
        // each primitive is its own mesh
        for (unsigned pid = 0; pid < gmesh.primitives_count; pid++) {
            cgltf_accessor *pos{nullptr}, *nor{nullptr}, *uv{nullptr};
            const auto& primitive = gmesh.primitives[pid];

            for (unsigned attid = 0; attid < primitive.attributes_count; attid++) {
                const auto& attribute = primitive.attributes[attid];

                switch (attribute.type) {
                    case cgltf_attribute_type_position: pos = attribute.data; break;
                    case cgltf_attribute_type_normal: nor = attribute.data; break;
                    case cgltf_attribute_type_texcoord: uv = attribute.data; break;
                    default: 
                }
            }

            if (!pos || pos->count == 0) continue;

            vertices.reserve(vertices.size() + pos->count);
            for (unsigned vid = 0; vid < pos->count; vid++) {
                Mesh::Vertex &v = vertices.emplace();
                cgltf_accessor_read_float(pos, vid, &v.pos.x, 3);

                if (nor) cgltf_accessor_read_float(nor, vid, &v.nor.x, 3);
                if (uv) cgltf_accessor_read_float(uv, vid, &v.uv.x, 2);
            }

            if (primitive.indices) {
                indices.reserve(indices.size() + primitive.indices->count);
                for (unsigned iid = 0; iid < primitive.indices->count; iid++) {
                    unsigned int intermediate;
                    cgltf_accessor_read_uint(primitive.indices, iid, &intermediate, 1);
                    indices.push(intermediate);
                }
            } else {
                indices.reserve(vertices.size());
                for (unsigned vid = 0; vid < vertices.size(); vid++) indices.push(vid);
            }
        }
    }
}

