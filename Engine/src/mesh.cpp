#include <PK/Util/file.hpp>
#include <PK/Render/Mesh.hpp>
#include <PK/Physics/Body.hpp>
#include <PK/Math/clamp.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <utility>

using namespace pk;
using std::string_view;
using std::string;

bool Mesh::loaded() const noexcept { return VBO && EBO; }
void Mesh::refresh() { if (loaded()) { unload(); load(); }}

void Mesh::load() {
    invInertia = vec3{0.f};
    
    if (!vtx.empty()) {
        vec3 minv = vtx[0].pos, maxv = vtx[0].pos;
        vec3 mean{0.f};

        for (const Vertex& v : vtx) mean += v.pos;
        mean /= vtx.size();

        for (Vertex& v : vtx) {
            v.pos -= mean;
            minv = pk::min(minv, v.pos);
            maxv = pk::max(maxv, v.pos);
        }

        box = {.min = minv, .max = maxv};
        invInertia = inverseInertia(vtx);
    }

    if (loaded()) return;

    VBO = buffer::generate();
    EBO = buffer::generate();
    VBO.upload(bufferTar::array, bufferUse::staticUsage, vtx);
    EBO.upload(bufferTar::element, bufferUse::staticUsage, ind);
}

void Mesh::unload() {
    VBO.destroy();
    EBO.destroy();
}

vec3 Mesh::getEdge(uint V0, uint V1) noexcept {
    return vtx[V1].pos - vtx[V0].pos;
}

float Mesh::getArea(uint V0, uint V1, uint V2) noexcept {
    return .5f * length(cross(getEdge(V0, V1), getEdge(V1, V2)));
}

float Mesh::getVolume(uint a, uint b, uint c, uint d) noexcept {
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
    box(b.box),
    invInertia(b.invInertia)
{}

Mesh& Mesh::operator=(Mesh&& b) noexcept {
    if (this == &b) return *this;
    if (loaded()) unload();

    vtx = std::move(b.vtx);
    ind = std::move(b.ind);
    material = b.material;
    box = b.box;
    invInertia = b.invInertia;
    VBO = std::move(b.VBO);
    EBO = std::move(b.EBO);
    return *this;
}

Mesh& Mesh::operator=(const Mesh& b) noexcept {
    if (this == &b) return *this;
    if (loaded()) unload();

    vtx = b.vtx;
    ind = b.ind;
    material = b.material;
    box = b.box;
    invInertia = b.invInertia;

    return *this;
}

Mesh::Mesh(const Mesh& b) noexcept: 
    material{b.material},
    vtx{b.vtx},
    ind{b.ind},
    box{b.box},
    invInertia{b.invInertia}
{}

Mesh::~Mesh() noexcept { unload(); }