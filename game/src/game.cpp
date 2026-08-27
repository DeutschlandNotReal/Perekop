//#include <PKLib/file.hpp>
//#include <PKLib/json.hpp>

#include <PK/callbacks.hpp>
#include <PK/world.hpp>
#include <PK/file.hpp>

#include <cstdio>
#include <PKGame/camera.hpp>
using namespace pk;

const path assets = path("game") / "assets";

void Perekop::on_step(f64 dt) {
    pkgame::step::camera(dt);
}

void Perekop::on_launch() {
    printf("Game begin\n");
    pkgame::init::camera();

    printf("Heh, it's gonna load...\n");
    World::import_scene(assets / "models/Untitled.glb");
    printf("Ouu shi it loaded %i meshes now\n", World::meshes.size());
    auto monkey = World::meshes.front();
    

    Shader* shader = new Shader(assets / "shaders/vert.glsl", assets / "shaders/frag.glsl");

    Texture* texture = new Texture(assets / "images/test.jpg");
    auto a = World::models.insert();
    for (int x = 0; x < 10; x++) for (int y = 0; y < 10; y++) for (int z = 0; z < 10; z++) {
        auto model = World::models.insert();
        model->mesh_id = monkey->id;
        model->pose.pos = vec3{x, y, z} * 10.f;
        model->scale = vec3{sin(x) + 1, cos(y) + 1, 1};
    }

    float rvcount = 1.f / static_cast<float>(monkey->vertices.size());
    for (unsigned vid = 0; vid < monkey->vertices.size(); vid++) {
        monkey->vertices[vid].uv = vec2{(float)vid} * rvcount;
    }

    a->mesh_id = monkey->id;
    monkey->shader = shader; 
    monkey->texture = texture;
    
    printf("%i indices, %i vertices\n", monkey->indices.size(), monkey->vertices.size());
}

void Perekop::on_exit() {
    printf("game's gone\n");
} 