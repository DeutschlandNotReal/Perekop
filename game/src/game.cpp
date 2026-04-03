#include "Perekop/Mesh.hpp"
#include <GLFW/glfw3.h>
#include <Perekop/Engine.hpp>
#include <Perekop/Time.hpp>

#include <iostream>

using namespace Perekop;
using namespace pk;

inline glm::vec3 cosxy(float theta) {
    glm::vec3 pos(0);
    sincosf(glm::radians(theta), &pos.x, &pos.z);
    return pos;
}

float camera_pitch = 0, camera_yaw = 0;
int i = 0;
bool on_step(double dt) {
    ++i;
    std::cout << dt*1000 << " ms\n";
    glm::vec3 disp(0);

    if (Input::is_key_down(GLFW_KEY_W)) disp += glm::vec3(0, 0, 1);
    if (Input::is_key_down(GLFW_KEY_S)) disp -= glm::vec3(0, 0, 1);
    if (Input::is_key_down(GLFW_KEY_D)) disp += glm::vec3(1, 0, 0);
    if (Input::is_key_down(GLFW_KEY_A)) disp -= glm::vec3(1, 0, 0);
    if (Input::is_key_down(GLFW_KEY_E)) disp += glm::vec3(0, 1, 0);
    if (Input::is_key_down(GLFW_KEY_Q)) disp -= glm::vec3(0, 1, 0);

    Scene::camera.transform.displace_local(disp * float(dt));
    return true;
}

void Game::launch() {
    std::cout << "game's so back\n";

    StackTimer<double, 10> timer;
    MeshMaterial chudmat(
        "void main() {gl_Position = VP * model() * vec4(_pos, 1.0); }",
        "void main() {fragColor = vec4(1.0, 1.0, 1.0, 1.0); }"
    );

    Mesh* pyramidler = Scene::new_mesh(chudmat); 
    pyramidler->push_vertex(0, 1, 0);
    pyramidler->push_vertex(cosxy(0));
    pyramidler->push_vertex(cosxy(120));
    pyramidler->push_vertex(cosxy(240));
    pyramidler->push_triangle(1, 2, 3);
    pyramidler->push_triangle(0, 2, 3);
    pyramidler->push_triangle(0, 3, 1);
    pyramidler->push_triangle(0, 1, 2);

    pyramidler->refresh();
    Scene::camera.transform.pos = glm::vec3(112, 112, -200);

    timer.push();
    for (int x = 0; x < 15; x++) {
        for (int y = 0; y < 15; y++) {
            for (int z = 0; z < 15; z++) {
                Model* M = new Model(pyramidler);
                M->transform = glm::vec3(x, y, z) * 25.f;
                //M->scl = {10, 10, 10};
            }
        }
    }
    timer.pop_log("mesh create: ");

    Window::step.listen(on_step);
}

void Game::close() {
    std::cout << "game's gone\n";
}  