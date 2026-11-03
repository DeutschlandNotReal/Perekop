#define PK_INTERNAL
#include <common.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
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
        vec3 mean{0.f};

        for (const Vertex& v : vtx) mean += v.pos;
        mean /= vtx.size();

        for (Vertex& v : vtx) {
            v.pos -= mean;
            minv = min(minv, v.pos);
            maxv = max(maxv, v.pos);
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

vec3 Mesh::GetEdge(uint V0, uint V1) noexcept {
    return vtx[V1].pos - vtx[V0].pos;
}

float Mesh::GetArea(uint V0, uint V1, uint V2) noexcept {
    return .5f * length(cross(GetEdge(V0, V1), GetEdge(V1, V2)));
}

float Mesh::GetVolume(uint a, uint b, uint c, uint d) noexcept {
    return (1.f / 6.f) * (
        dot(vtx[a].pos, cross(vtx[b].pos, vtx[c].pos)) +
        dot(vtx[b].pos, cross(vtx[c].pos, vtx[d].pos))
    );
}

Mesh::Mesh(Mesh&& b) noexcept: 
    VBO(std::move(b.VBO)),
    EBO(std::move(b.EBO)),
    material(b.material),
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
    material = b.material;
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
    material = b.material;
    centre = b.centre;
    invInertia = b.invInertia;
    ext = b.ext;
  
    return *this;
}

Mesh::Mesh(const Mesh& b) noexcept: 
    material{b.material},
    vtx{b.vtx},
    ind{b.ind},
    centre{b.centre},
    ext{b.ext},
    invInertia{b.invInertia}
{}

Mesh::~Mesh() noexcept { Unload(); }

Mesh Mesh::Merge(const Mesh& A, const Mesh& B, pose pA, pose pB) noexcept {
    Mesh merged;
    merged.ind.reserve(A.ind.size() + B.ind.size());
    merged.vtx.reserve(A.vtx.size() + B.vtx.size());
    pose dlt = localspace(pB, pA);

    for (const Vertex& v : A.vtx) merged.vtx.emplace(v);
    for (const Vertex& v : B.vtx) {
        merged.vtx.emplace(worldspace(v.pos, dlt), worldspace(v.nor, dlt), v.uv);
    };

    for (uint i : A.ind) merged.ind.emplace(i);
    for (uint i : B.ind) merged.ind.emplace(i+A.vtx.size());

    return merged;
}