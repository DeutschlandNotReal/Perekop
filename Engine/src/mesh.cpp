#include <PK/Util/file.hpp>
#include <PK/Render/Mesh.hpp>
#include <PK/Physics/Body.hpp>
#include <PK/Math/extra.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <utility>

using namespace pk;
using std::string_view;
using std::string;

bool Mesh::loaded() const noexcept { return VBO.getIndex() && EBO.getIndex(); }
void Mesh::refresh() { if (loaded()) { unload(); load(); }}

void Mesh::load() {
    invInertia = vec3{0.f};
    
    if (!vtx.empty()) {
        vec3 l, h;
        l = h  = vtx[0].pos;

        for (uint vid{1}; vid < vtx.size(); vid++) {
            vec3 p = vtx[vid].pos;
            l = min(l, p);
            h = max(h, p);
        }

        box = {.min = l, .max = h};
        invInertia = inverseInertia(vtx);
    }

    if (loaded()) return;

    VBO.generate();
    EBO.generate();

    VBO.upload(BufferTar::array, BufferUse::staticUsage, vtx);
    EBO.upload(BufferTar::element, BufferUse::staticUsage, ind);
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