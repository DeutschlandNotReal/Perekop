#include <Perekop.hpp>
#include <game/cam.hpp>
#include <GLFW/glfw3.h>

using namespace pk;
using namespace Perekop;
using namespace glm;
using World::camera;

vec2 cangle{0};
void Game::cam_init() {
    Mouse::on_scroll.listen([](auto d){
        camera.pose.pos += vec3(0,0,-d) * Mouse::matrix();
    });

    Mouse::on_move.listen([](auto movement){
        if (Mouse::held(Mouse::left) && !Gui::top) {
            vec2 size = Window::get_size();
            float yfov = camera.fov * (size.y / size.x);

            cangle += movement * vec2{camera.fov, yfov};
            camera.pose.rot = 
                angleAxis(radians(cangle.x), vec3{0,1,0}) *
                angleAxis(radians(cangle.y), vec3{1,0,0});
        }
    });

    Mouse::on_down.listen([](auto button){
        if (button == Mouse::left && !Gui::top) Mouse::lock();
    });

    Mouse::on_up.listen([](auto button){
        if (button == Mouse::left && !Gui::top) Mouse::unlock();
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
