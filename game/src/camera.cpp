#include <Perekop/world.hpp>
#include <Perekop/input.hpp>
#include <Perekop/gui.hpp>
#include <Perekop/window.hpp>
#include <PKGame/camera.hpp>

using namespace pk;
using namespace Perekop;
using namespace glm;

float _pitch{0}, _yaw{0};

void pkgame::init::camera() {
    Mouse::on_scroll.listen([](auto d){
        World::camera.pose.pos +=  Mouse::matrix() * vec3(0,0,-d);
    });

    Mouse::on_move.listen([](const vec2& delta){
        if (Mouse::held(Mouse::left) && !Gui::top) {
            vec2 size = Window::get_size();
            float yfov = World::camera.fov * (size.y / size.x);

            _pitch = clamp(_pitch - delta.y * yfov, radians(-60.f), radians(60.f));
            _yaw -= delta.x * World::camera.fov;

            World::camera.pose.rot = 
                angleAxis(_yaw, vec3{0,1,0}) *
                angleAxis(_pitch, vec3{1,0,0});
        }
    });

    Mouse::on_down.listen([](auto button){
        if (button == Mouse::left && !Gui::top) Mouse::lock();
    });

    Mouse::on_up.listen([](auto button){
        if (button == Mouse::left && !Gui::top) Mouse::unlock();
    });

}

void pkgame::step::camera(float dt) {
    using Input::held;
    vec3 delta{0};

    if (held('S')) delta += vec3{0,0,1};
    if (held('W')) delta -= vec3{0,0,1};
    if (held('D')) delta += vec3{1,0,0};
    if (held('A')) delta -= vec3{1,0,0};
    if (held('E')) delta += vec3{0,1,0};
    if (held('Q')) delta -= vec3{0,1,0};

    World::camera.pose += Mouse::matrix() * (delta * dt);
};
