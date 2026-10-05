#define PK_INTERNAL
#include <common.hpp>
#include <PK/Render/renderer.hpp>
#include <PK/Scene/collections.hpp>
#include <PK/Render/lighting.hpp>

using namespace pk;
using namespace Perekop;

span<Model> Collection::GetModels() noexcept {
    if (!data) return span<Model>();

    return span<Model>((Model*)data, ModelCount);
}

span<const Model> Collection::GetModels() const noexcept {
    if (!data) return span<const Model>();

    return span<const Model>((const Model*)data, ModelCount);
}

span<unsigned> Collection::GetMeshIds() noexcept {
    if (!data) return span<unsigned>();

    return span<unsigned>(
        (unsigned*) ((Model*)data + Capacity), 
        ModelCount
    );
}

span<const unsigned> Collection::GetMeshIds() const noexcept {
    if (!data) return span<const unsigned>();

    return span<const unsigned>(
        (const unsigned*) ((Model*)data + Capacity), 
        ModelCount
    );
}

void Collection::Resize(unsigned newcapacity) {
    void* newdata = pk::alloc<>(newcapacity * (sizeof(Model) + sizeof(unsigned)));
    pk::copy((Model*)newdata, (Model*)data, ModelCount);
    pk::copy((unsigned*)((Model*)newdata + newcapacity), (unsigned*)((Model*)data + Capacity), ModelCount);
    pk::free((char*) data);

    data = newdata;
    Capacity = newcapacity;
}

void Collection::AddModel(Model model, unsigned meshId) {
    if (ModelCount == Capacity)
        Resize(Capacity * 2 + 1);

    Model* models = (Model*)data;
    unsigned* meshids = (unsigned*)(models + Capacity);
    new (models + ModelCount) Model(std::move(model));
    meshids[ModelCount++] = meshId;
}

Collection::Collection(Collection&& collection) noexcept:
    data(std::exchange(collection.data, nullptr)),
    ModelCount(std::exchange(collection.ModelCount, 0)),
    Capacity(std::exchange(collection.Capacity, 0)),
    VAO(std::move(collection.VAO))
{}

Collection& Collection::operator=(Collection&& collection) noexcept {
    if (this == &collection) return *this;
    this->~Collection();
    data = std::exchange(collection.data, nullptr);
    ModelCount = std::exchange(collection.ModelCount, 0);
    Capacity = std::exchange(collection.Capacity, 0);
    VAO = std::move(collection.VAO);
    return *this;
}

Collection::Collection(const Collection& collection):
    VAO(collection.VAO)
{
    if (!collection.ModelCount) return;
    Resize(collection.ModelCount);
    for (unsigned i = 0; i < collection.ModelCount; ++i)
        AddModel(collection.GetModels()[i], collection.GetMeshIds()[i]);
}

Collection& Collection::operator=(const Collection& collection) {
    if (this == &collection) return *this;
    this->~Collection();
    new (this) Collection(collection);
    return *this;
}

Collection::~Collection() {
    if (!data) return;
    for (Model& model : GetModels()) model.~Model();
    pk::free((char*)data);
}



void DrawCollection(const Collection& collection, const mat4& camera, bool shadowPass) noexcept {
    span<const unsigned> meshids = collection.GetMeshIds();
    span<const Model>    models  = collection.GetModels();
    
    for (unsigned i = 0; i < models.size(); i++) {
        if (meshids[i] >= World::meshes.size()) continue;
        const Mesh&   mesh = World::meshes[meshids[i]];
        const Model& model = models[i];
        if (!mesh.material) continue;

        Render::Shader& shader = shadowPass ? Lighting::Shadow::Shader : mesh.material->shader;
        shader.Use();
        shader.GetUniform("camera")  = camera;
        shader.GetUniform("light")  = (mat4) Lighting::Light;
        shader.GetUniform("lightPos") = Lighting::Light.pos;
        shader.GetUniform("model")  = (mat4) model.pose;
        shader.GetUniform("scale")  = model.scale;

        if (!shadowPass) {
            shader.SetTexture("T",      0, mesh.material->textures[0]);
            shader.SetTexture("shadow", 1, Lighting::Shadow::Texture);
        }

        MeshVAO.Bind();
        MeshVAO.BindBuffer(0, mesh.VBO, Render::BufferTarget::Array,   sizeof(Mesh::Vertex));
        MeshVAO.BindBuffer(0, mesh.EBO, Render::BufferTarget::Element, 0);
        
        Lighting::Renderer.DrawElems((int) mesh.indices.size());
    }
}

void Perekop::RenderStep() noexcept {
    const mat4 VP = Lighting::Camera.Projection() * Lighting::Camera.View();
    const mat4 L  = Lighting::Light;

    Lighting::Renderer.Target(Lighting::Shadow::Buffer);
    Lighting::Renderer.Fill({1.f, 1.f, 1.f});
    Lighting::Renderer.Clear((int)Render::ClearTarget::Depth);
    for (const Collection& collection : World::collections)
        DrawCollection(collection, L, true);

    Lighting::Renderer.Viewport();
    Lighting::Renderer.Fill({.15f, .15f, .15f});
    Lighting::Renderer.Clear((int)Render::ClearTarget::Depth | (int)Render::ClearTarget::Colour);
    for (const Collection& collection : World::collections)
        DrawCollection(collection, VP, false);

    Lighting::Renderer.Swap();
}