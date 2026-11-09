#define PK_INTERNAL
#include "./internal.hpp"

#include <PK/Util/file.hpp>
#include <PK/Render/mesh.hpp>
#include <PK/Physics/body.hpp>
#include <PK/Math/clamp.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <utility>

using namespace pk;
using std::string_view;
using std::string;

bool mesh::loaded() const noexcept { return VBO && EBO; }
void mesh::refresh() { if (loaded()) { unload(); load(); }}

void mesh::load() {
    invInertia = vec3{0.f};
    
    if (!vtx.empty()) {
        vec3 minv = vtx[0].pos, maxv = vtx[0].pos;
        vec3 mean{0.f};

        for (const vertex& v : vtx) mean += v.pos;
        mean /= vtx.size();

        for (vertex& v : vtx) {
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

void mesh::unload() {
    VBO.destroy();
    EBO.destroy();
}

vec3 mesh::getEdge(uint V0, uint V1) noexcept {
    return vtx[V1].pos - vtx[V0].pos;
}

float mesh::getArea(uint V0, uint V1, uint V2) noexcept {
    return .5f * length(cross(getEdge(V0, V1), getEdge(V1, V2)));
}

float mesh::getVolume(uint a, uint b, uint c, uint d) noexcept {
    return (1.f / 6.f) * (
        dot(vtx[a].pos, cross(vtx[b].pos, vtx[c].pos)) +
        dot(vtx[b].pos, cross(vtx[c].pos, vtx[d].pos))
    );
}

mesh::mesh(mesh&& b) noexcept: 
    VBO(std::move(b.VBO)),
    EBO(std::move(b.EBO)),
    material(b.material),
    vtx(std::move(b.vtx)),
    ind(std::move(b.ind)),
    box(b.box),
    invInertia(b.invInertia)
{}

mesh& mesh::operator=(mesh&& b) noexcept {
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

mesh& mesh::operator=(const mesh& b) noexcept {
    if (this == &b) return *this;
    if (loaded()) unload();

    vtx = b.vtx;
    ind = b.ind;
    material = b.material;
    box = b.box;
    invInertia = b.invInertia;

    return *this;
}

mesh::mesh(const mesh& b) noexcept: 
    material{b.material},
    vtx{b.vtx},
    ind{b.ind},
    box{b.box},
    invInertia{b.invInertia}
{}

mesh::~mesh() noexcept { unload(); }