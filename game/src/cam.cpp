#include "glm/ext/quaternion_trigonometric.hpp"
#include <Perekop.hpp>
#include <cstdio>
#include <game/cam.hpp>
#include <GLFW/glfw3.h>

using namespace pk;
using namespace Perekop;
using namespace glm;
using World::camera;

vec2 cam_angle{0};

void Game::cam_init() {
    Mouse::on_scroll.listen([](int d){
        camera.pose.pos += vec3(0,0,-d) * Mouse::matrix();
    });

    Mouse::on_move.listen([](vec2 d){
        if (!Mouse::held(0) || GUI::top) return;
        float yfov = camera.fov * (Window::size.y / Window::size.x);

        cam_angle += d * vec2{camera.fov, yfov} / Window::size;
        camera.pose.rot = 
            angleAxis(radians(cam_angle.x), vec3{0,1,0}) *
            angleAxis(radians(cam_angle.y), vec3{1,0,0});
    });

    Mouse::on_down.listen([](int button){
        if (button == 0) Mouse::set(Mouse::captured);
    });

    Mouse::on_up.listen([](int button){
        if (button == 0) Mouse::set(Mouse::normal);
    });
}

using Input::held;
void Game::cam_step(float dt) {
    vec3 delta{0};

    if (held('S')) delta += vec3{0,0,1};
    if (held('W')) delta -= vec3{0,0,1};
    if (held('D')) delta += vec3{1,0,0};
    if (held('A')) delta -= vec3{1,0,0};
    if (held('E')) delta += vec3{0,1,0};
    if (held('Q')) delta -= vec3{0,1,0};

    camera.pose += (delta * dt) * Mouse::matrix();
};
