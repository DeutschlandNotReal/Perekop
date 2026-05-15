#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <Perekop/Engine.hpp>
#include <Perekop/Time.hpp>
#include <iostream>

using namespace pk;
using glm::vec3;

static Window& window = Perekop::main_window;

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

void Perekop::on_step(float dt) {
    vec3 disp{0, 0, 0};

    for (const KeyInfluence& infl : influences) 
        if (window.keyboard.key_held(infl.key)) disp += infl.influence;

    Perekop::camera.transform.displace_l(disp * dt);
}

void Perekop::on_launch() {
    std::cout << "Game begin\n";
    StackTimer<double, 10> timer;

    MeshMaterial chudmat(
        "\n"
        "\n out vec4 col; "
        "\n void main() { gl_Position = VP * model() * vec4(_pos, 1.0); col = vec4(_uv.x, _uv.y, 0.0, 1.0); }",
        "in vec4 col; void main() { fragColor = col; }"
    );

    Mesh cubeler{chudmat};
    cubeler.vertex.push({
        {{0, 0, 0}, {0, 0}},
        {{1, 0, 0}, {1, 0}},
        {{1, 1, 0}, {1, 1}},
        {{0, 1, 0}, {0, 1}},
        {{0, 0, 1}, {0, 0}},
        {{1, 0, 1}, {1, 0}},
        {{1, 1, 1}, {1, 1}},
        {{0, 1, 1}, {0, 1}}
    });

    cubeler.triangle.push({
        {0, 1, 2}, {0, 2, 3},
        {4, 6, 5}, {4, 7, 6},
        {0, 4, 5}, {0, 5, 1},
        {1, 5, 6}, {1, 6, 2},
        {2, 6, 7}, {2, 7, 3},
        {3, 7, 4}, {3, 4, 0}
    });

    cubeler.refresh();
    auto cubeler_id = Perekop::scene.register_mesh(cubeler);

    for (int x = 0; x < 3; x++) 
        for (int y = 0; y < 3; y++) 
            for (int z = 0; z < 3; z++) {
                Model model{{x, y, -z}};
                auto model_id = Perekop::scene.register_model(model);
                Perekop::scene.link_model(cubeler_id, model_id);
            }
}

void Perekop::on_close() {
    std::cout << "game's gone\n";
} 