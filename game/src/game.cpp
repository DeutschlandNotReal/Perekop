#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <pk/Engine.hpp>
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

    camera.transform.displace_l(disp * dt);
    camera.transform.rotate_axis({0, 1, 0}, dt * 0.2);
}

void Perekop::on_launch() {
    std::cout << "Game begin\n";

    MeshMaterial chudmat(
        "\n"
        "\n out vec4 col; "
        "\n void main() { gl_Position = VP * model() * vec4(_pos, 1.0); col = vec4(_uv.x, _uv.y, 0.0, 1.0); }",
        "in vec4 col; void main() { fragColor = col; }"
    );

    Mesh& cubeler = scene.create_mesh();
    cubeler.mat = chudmat;
    cubeler.vertex = {
        {{0, 0, 0}, {0, 0}},
        {{1, 0, 0}, {1, 0}},
        {{1, 1, 0}, {1, 1}},
        {{0, 1, 0}, {0, 1}},
        {{0, 0, 1}, {0, 0}},
        {{1, 0, 1}, {1, 0}},
        {{1, 1, 1}, {1, 1}},
        {{0, 1, 1}, {0, 1}}
    };

    cubeler.indices = {
        0,2,1, 0,3,2, // front
        4,5,6, 4,6,7, // back
        0,1,5, 0,5,4, // bottom
        1,2,6, 1,6,5, // right
        2,3,7, 2,7,6, // top
        3,0,4, 3,4,7  // left
    };

    //cubeler.rewind();
    cubeler.refresh();

    for (int x = -3; x < 3; x++) 
        for (int y = -3; y < 3; y++) 
            for (int z = -3; z < 3; z++) {
                Model& model = scene.create_model();
                model.transform = vec3{x, y, z} * 10.f;
                scene.link(cubeler.id, model.id);
            }
}

void Perekop::on_close() {
    std::cout << "game's gone\n";
} 