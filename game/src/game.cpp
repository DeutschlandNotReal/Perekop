#include "GLFW/glfw3.h"
#include "Perekop/Geometry.hpp"
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
    glm::vec2 delta = (Mouse::normalized_pos() - glm::vec2(0.5)) * float(dt);
    if (Mouse::is_rmb_down()) {
        camera_pitch = glm::clamp(camera_pitch + delta.y, -2.f, 2.f);
        camera_yaw += delta.x;
        Scene::camera.transform.setYXZ(camera_yaw, camera_pitch, 0);
        Mouse::set_normalized_pos(glm::vec2(0.5));
    }
    glm::vec3 disp(0);

    if (Input::is_key_down(GLFW_KEY_W)) disp += glm::vec3(0, 0, 1);
    if (Input::is_key_down(GLFW_KEY_S)) disp -= glm::vec3(0, 0, 1);
    if (Input::is_key_down(GLFW_KEY_D)) disp += glm::vec3(1, 0, 0);
    if (Input::is_key_down(GLFW_KEY_A)) disp -= glm::vec3(1, 0, 0);
    if (Input::is_key_down(GLFW_KEY_E)) disp += glm::vec3(0, 1, 0);
    if (Input::is_key_down(GLFW_KEY_Q)) disp -= glm::vec3(0, 1, 0);

    Scene::camera.transform.displace_local(disp * float(dt));

    glm::vec3 look = Scene::camera.transform.rot[0];
    std::cout << "( " << look.x << ", " << look.y << ", " << look.z << ")\n";
    return true;
}

void Game::launch() {
    std::cout << "game's so back\n";

    Mesh* pyramidler = Scene::new_mesh();
    pyramidler->push_vertex(0, 1, 0);
    pyramidler->push_vertex(cosxy(0));
    pyramidler->push_vertex(cosxy(120));
    pyramidler->push_vertex(cosxy(240));
    pyramidler->push_triangle(1, 2, 3);
    pyramidler->push_triangle(0, 2, 3);
    pyramidler->push_triangle(0, 3, 1);
    pyramidler->push_triangle(0, 1, 2);

    pyramidler->load();
    pyramidler->flush();

    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 10; y++) {
            for (int z = 0; z < 10; z++) {
                Model* M = new Model(pyramidler);
                M->transform = glm::vec3(x, y, z) * 25.f;
            }
        }
    }

    Window::step.connect(on_step);
}

void Game::close() {
    std::cout << "game's gone\n";
}  