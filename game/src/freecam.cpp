#include "glm/ext/quaternion_trigonometric.hpp"
#include <Perekop.hpp>
#include <game/freecam.hpp>
#include <GLFW/glfw3.h>

using namespace pk;
using namespace Perekop;
using namespace glm;
using World::camera;

const vec3 x(1,0,0), y(0,1,0), z(0,0,1);
vec2 cam_angle{0};

mat3 mouse_matrix() {
    vec3 f = Mouse::fvec();
    vec3 r = normalize(cross(f, y));
    vec3 u = normalize(cross(r, f));
    return {r, u, f};
};

void Game::freecam_init() {
    Mouse::on_scroll.listen([](int d){
        camera.pose.pos += vec3(0,0,-d) * mouse_matrix();
    });

    Mouse::on_move.listen([](vec2 d){
        if (!Mouse::held(0)) return;
        vec2 winsize = Window::size();
        float yfov = camera.fov * (winsize.y / winsize.x);

        cam_angle += d * vec2{camera.fov, yfov} / winsize;
        camera.pose.rot = 
            angleAxis(radians(cam_angle.x), y) *
            angleAxis(radians(cam_angle.y), z);
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

    if (held('S')) delta += z;
    if (held('W')) delta -= z;
    if (held('D')) delta += x;
    if (held('A')) delta -= x;

    camera.pose.pos += delta * dt * mouse_matrix();
};
