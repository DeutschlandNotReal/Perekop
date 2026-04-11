#include "Perekop/Mesh.hpp"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <Perekop/Engine.hpp>
#include <Perekop/Time.hpp>

#include <iostream>

using namespace Perekop;
using namespace pk;

using glm::vec3;

inline vec3 cosxy(float theta) {
    vec3 pos(0);
    sincosf(glm::radians(theta), &pos.x, &pos.z);
    return pos;
}

struct KeyInfluence {
    GLenum key;
    vec3 influence;
    KeyInfluence(GLenum K, float x, float y, float z): key(K), influence(x, y, z) {}
};

KeyInfluence influences[6] = {
    {GLFW_KEY_W,  0,  0,  1},
    {GLFW_KEY_S,  0,  0, -1},
    {GLFW_KEY_D,  1,  0,  0},
    {GLFW_KEY_A, -1,  0,  0},
    {GLFW_KEY_E,  0,  1,  0},
    {GLFW_KEY_Q,  0, -1,  0}
};

bool on_step(double dt) {
    std::cout << (1.0 / dt) << " FPS\n";
    vec3 delta;

    for (const auto& infl : influences) {
        if (Input::is_key_down(infl.key)) delta += infl.influence;
    }

    Scene::camera.transform.displace_local(delta * float(dt) * 5.f);
    return true;
}

void Game::launch() {
    std::cout << "game's so back?\n";
    StackTimer<double, 10> timer;
    MeshMaterial chudmat(
        "\n"
        "\n out vec4 col; "
        "\n void main() { gl_Position = VP * model() * vec4(_pos, 1.0); col = vec4(_uv.x, _uv.y, 0.0, 1.0); }",
        "in vec4 col; void main() {  fragColor = col; }"
    );

    Mesh* pyramidler = Scene::new_mesh(chudmat); 
    auto& vertex = pyramidler->vertex;
    auto& triangle = pyramidler->triangle;

    vertex.emplace(0, 1, 0);

    vertex.emplace(cosxy(0));
    vertex.emplace(cosxy(120));
    vertex.emplace(cosxy(240));

    triangle.emplace(1, 2, 3);
    triangle.emplace(0, 2, 3);
    triangle.emplace(0, 3, 1);
    triangle.emplace(0, 1, 2);

    auto bounds = pyramidler->bounds();
    glm::vec3 range = bounds.max - bounds.min;
    for (int i = 0; i < vertex.length(); i++) {
        vertex[i].uv = (vertex[i].pos - bounds.min) / range;
    }

    pyramidler->refresh();
    Scene::camera.transform.pos = glm::vec3(112, 112, -200);

    timer.push();
    for (int x = 0; x < 15; x++) {
        for (int y = 0; y < 15; y++) {
            for (int z = 0; z < 15; z++) {
                new Model(pyramidler, vec3(x, y, z) * 100.f);
            }
        }
    }
    timer.pop_log("mesh create");

    Window::step.listen(on_step);
}

void Game::close() {
    std::cout << "game's gone\n";
}  