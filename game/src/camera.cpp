#include <PK/world.hpp>
#include <PK/userinput.hpp>
#include <PK/window.hpp>
#include <PKGame/camera.hpp>

using namespace pk;
using namespace Perekop;
float _pitch{0}, _yaw{0};

void pkgame::init::camera() {
    vec3 dir = normalize(World::camera.pose.rot * vec3{0,0,-1});;

    _yaw = atan2(-dir.x, -dir.z);
    _pitch = asin(dir.y);

    Mouse::on_scroll.listen([](auto d){
        World::camera.pose += World::camera.pose.rot * vec3{0, 0, -d};
    });

    Mouse::on_move.listen([](vec2 delta){
        if (Mouse::held(Mouse::left) /* && !Gui::top */ ) {
            vec2 size = Window::size();
            float rfov = radians(World::camera.fov());
            float yfov = rfov * size.x / size.y;

            _pitch = clamp(_pitch - delta.y * yfov, radians(-60.f), radians(60.f));
            _yaw -= delta.x * rfov;

            World::camera.pose.rot = angleAxis(_yaw, vec3{0, 1, 0}) * angleAxis(_pitch, vec3{1, 0, 0});
        }
    });

    Mouse::on_down.listen([](auto button){
        // if (button == Mouse::left && !Gui::top) Mouse::lock();
    });

    Mouse::on_up.listen([](auto button){
        //if (button == Mouse::left && !Gui::top) Mouse::unlock();
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

    World::camera.pose += World::camera.pose.rot * (3.f * delta * dt);
};
