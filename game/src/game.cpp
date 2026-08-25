//#include <PKLib/file.hpp>
//#include <PKLib/json.hpp>

#include <PK/callbacks.hpp>
#include <PK/world.hpp>
#include <PK/file.hpp>

#include <cstdio>
#include <PKGame/camera.hpp>
using namespace pk;

const std::filesystem::path modelpath = "game/assets/models";

void Perekop::on_step(f64 dt) {
    pkgame::step::camera(dt);
}

void Perekop::on_launch() {
    printf("Game begin\n");
    pkgame::init::camera();

    Mesh& monkey = World::import_mesh(modelpath / "Untitled.stl");

    printf("%i indices, %i vertices\n", monkey.indices.size(), monkey.vertices.size());
}

void Perekop::on_exit() {
    printf("game's gone\n");
} 