#include "glm/ext/quaternion_trigonometric.hpp"
#include <iostream>
#include <pk/Engine.hpp>
#include <game/freecam.hpp>

using namespace pk;
using namespace Perekop;
using namespace glm;

const double scroll_k = 3;
const double angle_k = 0.4;

vec2 cam_angle{0};

void Game::freecam_init() {
    window.input.on_scroll_m.listen([](double d){
        camera.pose.displace_local({0, 0, -d*scroll_k});
    });

    window.input.on_move_m.listen([](vec2 d){
        if (!window.input.is_held_m(0)) return;
        vec2 winsize = window.size();
        float yfov = camera.fov * (winsize.y / winsize.x);
        vec2 angles = d / winsize * vec2{camera.fov, yfov} * (float)angle_k;

        cam_angle += angles;
        camera.pose.rot = 
            angleAxis(radians(cam_angle.x), vec3{0,1,0}) *
            angleAxis(radians(cam_angle.y), vec3{0,0,1});
    });

    window.input.on_down_m.listen([](int button){
        if (button == 0) window.input.mouse_mode(Window::Input::captured);
    });

    window.input.on_up_m.listen([](int button){
        if (button == 0) window.input.mouse_mode(Window::Input::normal);
    });
}

void Game::freecam_step(float dt) {
    vec3 delta;
    auto& i = window.input;
    if (i.is_held_k(GLFW_KEY_W))
        delta += vec3{0, 0, -1};
    if (i.is_held_k(GLFW_KEY_S))
        delta += vec3(0, 0, 1);
    if (i.is_held_k(GLFW_KEY_D))
        delta += vec3(1, 0, 0);
    if (i.is_held_k(GLFW_KEY_A))
        delta += vec3(-1, 0, 0);
    camera.pose.displace_local(delta * dt);
    std::cout << "(" << camera.pose.pos.x << ", " << camera.pose.pos.y << ", " << camera.pose.pos.z << ")\n";
}