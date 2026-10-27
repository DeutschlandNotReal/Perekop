#define PK_INTERNAL
#include <common.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <cgltf.h>
#include <PK/Util/file.hpp>
#include <utility>
#include <PK/Render/mesh.hpp>
#include <PK/Physics/body.hpp>

using namespace pk;
using std::string_view;
using std::string;

Render::VertexArray pk::MeshVAO;

bool Mesh::Loaded() const noexcept { return VBO && EBO; }
void Mesh::Refresh() { if (Loaded()) { Unload(); Load(); }}

void Mesh::Load() {
    ext = centre = invInertia = vec3{0.f};
    
    if (!vtx.empty()) {
        vec3 minv = vtx[0].pos, maxv = vtx[0].pos;

        for (const Vertex& vertex : vtx) {
            minv = min(minv, vertex.pos);
            maxv = max(maxv, vertex.pos);
        }

        centre = (maxv + minv) * .5f;
        ext = centre - minv;
        invInertia = Physics::InverseInertia(vtx);
    }

    if (Loaded()) return;

    VBO = Render::Buffer::Generate();
    EBO = Render::Buffer::Generate();
    VBO.Upload(Render::BufferTarget::Array,   Render::BufferUsage::Static, vtx);
    EBO.Upload(Render::BufferTarget::Element, Render::BufferUsage::Static, ind);
}

void Mesh::Unload() {
    VBO.Delete();
    EBO.Delete();
}

vec3 Mesh::GetEdge(unsigned V0, unsigned V1) noexcept {
    return vtx[V1].pos - vtx[V0].pos;
}

float Mesh::GetArea(unsigned V0, unsigned V1, unsigned V2) noexcept {
    return .5f * length(cross(GetEdge(V0, V1), GetEdge(V1, V2)));
}

float Mesh::GetVolume(unsigned a, unsigned b, unsigned c, unsigned d) noexcept {
    return (1.f / 6.f) * (
        dot(vtx[a].pos, cross(vtx[b].pos, vtx[c].pos)) +
        dot(vtx[b].pos, cross(vtx[c].pos, vtx[d].pos))
    );
}

Mesh::Mesh(Mesh&& b) noexcept: 
    VBO(std::move(b.VBO)),
    EBO(std::move(b.EBO)),
    vtx(std::move(b.vtx)),
    ind(std::move(b.ind)),
    centre(b.centre),
    ext(b.ext),
    invInertia(b.invInertia)
{}

Mesh& Mesh::operator=(Mesh&& b) noexcept {
    if (this == &b) return *this;
    if (Loaded()) Unload();

    vtx = std::move(b.vtx);
    ind = std::move(b.ind);
    centre = b.centre;
    ext = b.ext;
    invInertia = b.invInertia;
    VBO = std::move(b.VBO);
    EBO = std::move(b.EBO);
    return *this;
}

Mesh& Mesh::operator=(const Mesh& b) noexcept {
    if (this == &b) return *this;
    if (Loaded()) Unload();

    vtx = b.vtx;
    ind = b.ind;
    centre = b.centre;
    invInertia = b.invInertia;
    ext = b.ext;
  
    return *this;
}

Mesh::Mesh(const Mesh& b) noexcept: 
    vtx{b.vtx},
    ind{b.ind},
    centre{b.centre},
    invInertia{b.invInertia},
    ext{b.ext}
{}

Mesh::~Mesh() noexcept { Unload(); }

Mesh::Mesh(const std::filesystem::path& path) noexcept {
    cgltf_options options{};  
    cgltf_data* data{nullptr};
    auto spath = path.string();

    if (cgltf_parse_file(&options, spath.c_str(), &data) != cgltf_result_success)
        return;

    if (cgltf_load_buffers(&options, data, spath.c_str()) != cgltf_result_success) {
        cgltf_free(data);
        return;
    }

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
                    default: break;
                }
            }

            if (!pos || pos->count == 0) continue;

            vtx.reserve(vtx.size() + pos->count);
            for (unsigned vid = 0; vid < pos->count; vid++) {
                Mesh::Vertex& vertex = vtx.emplace();
                cgltf_accessor_read_float(pos, vid, &vertex.pos.x, 3);

                if (nor) cgltf_accessor_read_float(nor, vid, &vertex.nor.x, 3);
                if (uv) cgltf_accessor_read_float(uv, vid, &vertex.uv.x, 2);
            }

            if (primitive.indices) {
                unsigned index{};
                ind.reserve(ind.size() + primitive.indices->count);
                for (unsigned iid = 0; iid < primitive.indices->count; iid++) {
                    cgltf_accessor_read_uint(primitive.indices, iid, &index, 1);
                    ind.push(index);
                }
            } else {
                ind.reserve(vtx.size());
                for (unsigned vid = 0; vid < vtx.size(); vid++) ind.push(vid);
            }
        }
    }

    cgltf_free(data);
}
