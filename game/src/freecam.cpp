#include "glm/ext/quaternion_trigonometric.hpp"
#include <Perekop.hpp>
#include <game/freecam.hpp>
#include <GLFW/glfw3.h>

using namespace pk;
using namespace Perekop;
using namespace glm;
using World::camera;

vec2 cam_angle{0};
void Game::freecam_init() {
    Mouse::on_scroll.listen([](int d){
        camera.pose.displace_local({0, 0, -d});
    });

    Mouse::on_move.listen([](vec2 d){
        if (!Mouse::held(0)) return;
        vec2 winsize = Window::size();
        float yfov = camera.fov * (winsize.y / winsize.x);

        cam_angle += d * vec2{camera.fov, yfov} / winsize;
        camera.pose.rot = 
            angleAxis(radians(cam_angle.x), vec3{0,1,0}) *
            angleAxis(radians(cam_angle.y), vec3{0,0,1});
    });

    Input::on_down.listen([](int button){
        if (button == 0) Mouse::set(Mouse::captured);
    });

    Input::on_up.listen([](int button){
        if (button == 0) Mouse::set(Mouse::normal);
    });
}

using Input::held;
void Game::freecam_step(float dt) {
    vec3 delta{0};
    if (held('S')) delta += vec3(0,0,1);
    if (held('W')) delta -= vec3{0,0,1};
    if (held('D')) delta += vec3(1,0,0);
    if (held('A')) delta -= vec3(1,0,0);
    
    camera.pose.displace_local(delta * dt);
};
