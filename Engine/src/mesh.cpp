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

bool Mesh::loaded() const noexcept {
     return VBO.getIndex() && EBO.getIndex(); 
}

void Mesh::synctraits() noexcept {
    inertia = vec3{0.f};

    if (vtx.size()) {
        vec3 l, h;
        l = h  = vtx[0].pos;

        for (uint vid{1}; vid < vtx.size(); vid++) {
            vec3 p = vtx[vid].pos;
            l = min(l, p);
            h = max(h, p);
        }

        bounds = {.min = l, .max = h};
        inertia = inverseInertia(vtx);
    }
}
void Mesh::load() {
    if (loaded()) return;

    VBO = Buffer::create();
    EBO = Buffer::create();
 
    VBO.upload(BufferTar::array,   BufferUse::staticUsage, vtx);
    EBO.upload(BufferTar::element, BufferUse::staticUsage, ind);
}

void Mesh::unload() {
    EBO={}, VBO={};
}

Mesh::Mesh(Mesh&& b) noexcept: 
    VBO(std::move(b.VBO)),
    EBO(std::move(b.EBO)),
    material(b.material),
    vtx(std::move(b.vtx)),
    ind(std::move(b.ind)),
    tag(std::move(b.tag)),
    bounds(b.bounds),
    inertia(b.inertia)
{}

Mesh& Mesh::operator=(Mesh&& b) noexcept {
    if (this == &b) return *this;

    this->~Mesh();
    new (this) Mesh(std::move(b));
    
    return *this;
}