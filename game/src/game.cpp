#include <PKLib/file.hpp>
#include <PKLib/json.hpp>

#include <Perekop/callbacks.hpp>
#include <Perekop/world.hpp>

#include <cstdio>
#include <PKGame/camera.hpp>
using namespace pk;
using namespace glm;

void Perekop::on_step(double dt) {
    pkgame::step::camera(dt);
}

void Perekop::on_launch() {
    printf("Game begin\n");
    pkgame::init::camera();

    string src = read_file("game/assets/extra/components.json");
    pk::json::parse(src, [](const json::token& t, void*){
        using enum json::type;
        switch (t.type) {
            case _flt:
                printf("key \"%*s\": _flt (%.4f)\n", t.key.size(), t.key.begin(), t.value_flt);
            case _int:
                printf("key \"%*s\": _int (%i)\n", t.key.size(), t.key.begin(), t.value_int);
            case _str:
                printf("key \"%*s\": _str (\"%*s\")\n", t.key.size(), t.key.begin(), t.value_str.size(), t.value_str.begin());
            default:
                printf("key \"%*s\": token '%c'\n", t.key.size(), t.key.begin(), t.type);
        }
    });
}

void Perekop::on_exit() {
    printf("game's gone\n");
} 