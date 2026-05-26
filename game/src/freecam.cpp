#include <iostream>
#include <pk/Engine.hpp>
#include <game/freecam.hpp>

using namespace pk;
using namespace Perekop;
using namespace glm;

const double scroll_k = 3;
const double angle_k = 0.4;

void Game::freecam_init() {
    main_window.mouse.on_scroll.listen([](double d){
        camera.transform.displace_local({0, 0, -d*scroll_k});
    }); 

    main_window.mouse.on_move.listen([](vec2 d){
        if (!main_window.mouse.left_held()) return;
        vec2 winsize = main_window.size();
        float yfov = camera.fov * (winsize.y / winsize.x);
        vec2 angles = d / winsize * vec2{camera.fov, yfov} * (float)angle_k;
        camera.transform = camera.transform.rotate(angles.x, {0, 1, 0}).rotate(angles.y, {0, 0, -1});
    });

    main_window.mouse.left_down.listen([](){
        main_window.mouse.lock();
    });

    main_window.mouse.left_up.listen([](){
        main_window.mouse.reset();
    });
}

void Game::freecam_step(float dt) {
    vec3 delta;
    auto& kb = main_window.keyboard;
    if (kb.key_held(GLFW_KEY_W))
        delta += vec3{0, 0, -1};
    if (kb.key_held(GLFW_KEY_S))
        delta += vec3(0, 0, 1);
    if (kb.key_held(GLFW_KEY_D))
        delta += vec3(1, 0, 0);
    if (kb.key_held(GLFW_KEY_A))
        delta += vec3(-1, 0, 0);
    camera.transform.displace_local(delta * dt);
    auto& p = camera.transform.pos;
    std::cout << "(" << p.x << ", " << p.y << ", " << p.z << ")\n";
}