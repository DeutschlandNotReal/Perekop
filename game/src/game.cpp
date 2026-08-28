#include <PK/callbacks.hpp>
#include <PK/world.hpp>
#include <PK/file.hpp>
#include <PK/window.hpp>
#include <PK/shader.hpp>

#include <cstdio>
#include <PKGame/camera.hpp>
using namespace pk;

const path assets = path("game") / "assets";

void Perekop::on_step(f64 dt) {
    pkgame::step::camera(dt);
}

mat4 V{0}, P{0};
Mesh ball, other;
ShaderProgram shader;
vector<Model> models;
Texture texture;

void Perekop::on_render() {
    V = World::camera.view();
    P = World::camera.proj();
    Window::clear(World::bgcol);

    shader.draw(other, models);

    Window::swap_buffers();
}

void Perekop::on_launch() {
    printf("Game begin\n");
    Perekop::Window::icon(assets / "images/icon.ico");
    World::camera.pose = inverse(lookAt(vec3{25, 25, 25}, vec3{0, 0, 0}, vec3{0, 1, 0}));
    pkgame::init::camera();

    ball = assets / "models/Untitled.glb";
    other = assets / "models/PK67.glb";

    ball.load(); other.load();
    shader = ShaderProgram(
        Shader(Shader::vertex, assets / "shaders/vert.glsl"),
        Shader(Shader::fragment, assets / "shaders/frag.glsl"),
        {
            {.name = "V", .type = ShaderProgram::u_mat4, .data = &V},
            {.name = "P", .type = ShaderProgram::u_mat4, .data = &P},
            {.name = "T", .type = ShaderProgram::texture, .data = &texture}
        }
    );
    texture = assets / "images/test.jpg";

    for (int x = 0; x < 15; x++) {
        for (int y = 0; y < 15; y++) {
            for (int z = 0; z < 15; z++) {
                models.emplace(vec3{x, y, z} * 5.f, vec3{1, 1, 1}, vec4{x, y, z, x});
            }
        }
    }
}

void Perekop::on_exit() {
    printf("game's gone\n");
} 