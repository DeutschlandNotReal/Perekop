#include <PK/Interface/world.hpp>
#include <PK/Interface/input.hpp>
#include <PK/Interface/window.hpp>
#include <PKGame/camera.hpp>

using namespace pk;
using namespace Perekop;
f32 _pitch{0}, _yaw{0};

void pkgame::init::camera() {
    Mouse::on_scroll.listen([](auto d){
        World::camera.t += vec4{0, 0, -d, 0} * Mouse::transform.rot;
    });

    Mouse::on_move.listen([](vec2 delta){
        if (Mouse::held(Mouse::left) /* && !Gui::top */ ) {
            vec2 size = Window::get_size();
            f32 yfov = World::camera.fov * (size.y / size.x);

            _pitch = std::clamp(_pitch - delta.y * yfov, radians(-60.f), radians(60.f));
            _yaw -= delta.x * World::camera.fov;

            World::camera.t.rot = quat::axis_angle(vec3{0, 1, 0}, _yaw) * quat::axis_angle(vec3{1, 0, 0}, _pitch);
        }
    });

    Mouse::on_down.listen([](auto button){
        // if (button == Mouse::left && !Gui::top) Mouse::lock();
    });

    Mouse::on_up.listen([](auto button){
        //if (button == Mouse::left && !Gui::top) Mouse::unlock();
    });

}

void pkgame::step::camera(f32 dt) {
    using Input::held;
    vec3 delta{0};

    if (held('S')) delta += {0,0,1};
    if (held('W')) delta -= {0,0,1};
    if (held('D')) delta += {1,0,0};
    if (held('A')) delta -= {1,0,0};
    if (held('E')) delta += {0,1,0};
    if (held('Q')) delta -= {0,1,0};

    World::camera.t += Mouse::transform * (delta * (int)3);
};
