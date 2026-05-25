#include <iostream>

#include <pk/Engine.hpp>
#include <pkutil/File.hpp>

using namespace pk;
using glm::vec3;

static Window& window = Perekop::main_window;
static float t = 0;

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
    vec3 delta;
    t += dt;

    for (const KeyInfluence& infl : influences) 
        if (window.keyboard.key_held(infl.key)) 
            delta += infl.influence;

    camera.transform += delta * dt;
}

void Perekop::on_launch() {
    std::cout << "Game begin\n";
    Perekop::camera.transform.pos = {0, 0, 10.f};

    const char* vsrc = File::read("game/shaders/vert.glsl");
    const char* fsrc = File::read("game/shaders/frag.glsl");
    auto* chudmat = new Mesh::Material(vsrc, fsrc);
    delete[] vsrc;
    delete[] fsrc;

    Mesh& cubeler = scene.meshes.insert();
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

    chudmat->add_uniform(Mesh::u_float, "t", &t);

    cubeler.indices = {
        0,2,1, 0,3,2, // front
        4,5,6, 4,6,7, // back
        0,1,5, 0,5,4, // bottom
        1,2,6, 1,6,5, // right
        2,3,7, 2,7,6, // top
        3,0,4, 3,4,7  // left
    };

    cubeler.refresh();

    for (int x = -3; x < 3; x++) 
        for (int y = -3; y < 3; y++) 
            for (int z = -3; z < 3; z++) {
                Model& model = scene.models.insert();
                model.transform.pos = vec3{x, y, z} * 10.f;
                scene.link(cubeler.id, model.id);
            }
}

void Perekop::on_close() {
    std::cout << "game's gone\n";
} 