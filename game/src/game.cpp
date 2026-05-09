#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <Perekop/Engine.hpp>
#include <Perekop/Time.hpp>
#include <iostream>

using namespace pk;
using glm::vec3;

static Window& window = Perekop::main_window;

inline vec3 angle_XZ(float angle) {
    vec3 pos;
    sincosf(angle, &pos.z, &pos.x);
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

void Perekop::on_step(float dt) {
    vec3 camdisp;

    for (const KeyInfluence& infl : influences) 
        if (window.keyboard.is_held(infl.key)) camdisp += infl.influence;

    Perekop::camera.transform.displace_local(camdisp * float(dt) * 5.f);
}

void Perekop::on_launch() {
    std::cout << "Game begin\n";
    StackTimer<double, 10> timer;

    std::cout << "pre mat\n";
    MeshMaterial chudmat(
        "\n"
        "\n out vec4 col; "
        "\n void main() { gl_Position = VP * model() * vec4(_pos, 1.0); col = vec4(_uv.x, _uv.y, 0.0, 1.0); }",
        "in vec4 col; void main() {  fragColor = col; }"
    );

    std::cout << "pre new-mesh\n";
    Mesh& pyramidler = Perekop::create_mesh(chudmat); 
    auto& vertex = pyramidler.vertex;
    auto& triangle = pyramidler.triangle;

    vertex.push({
        {0, 1, 0},
        {angle_XZ(0)},
        {angle_XZ(120)},
        {angle_XZ(240)}
    });

    triangle.push({
        {1, 2, 3},
        {0, 2, 3},
        {0, 3, 1},
        {0, 1, 2}
    });

    auto bounds = pyramidler.bounds();
    vec3 range = bounds.max - bounds.min;

    for (MeshVertex& v : vertex)
        v.uv = (v.pos - bounds.min) / range;
    
    pyramidler.refresh();

    for (int x = 0; x < 5; x++) 
        for (int y = 0; y < 5; y++) 
            for (int z = 0; z < 5; z++) 
                new Model(&pyramidler, vec3(x, y, z) * 100.f);

}

void Perekop::on_close() {
    std::cout << "game's gone\n";
}  