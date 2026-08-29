#include <PK/userinput.hpp>
#include <PK/window.hpp>
#include <PKG/camera.hpp>
using namespace pk;
using namespace Perekop;

float campitch{0}, camyaw{0};

void PKG::InitCamera(Camera &camera) noexcept{
    vec3 look = normalize(camera.pose.rot * vec3{0, 0, -1});
    camyaw   = atan2(-look.x, -look.z);
    campitch = asin(look.y);

    Mouse::BindToScroll([&camera](auto x){
        camera.pose += camera.pose.rot * vec3{0,0,-x};
    });

    Mouse::BindToMove([&camera](vec2 delta){
        if (!Mouse::Held(Mouse::right)) return;

        vec2 size = Window::Size();
        delta /= size;
        float rfov = radians(camera.fov());
        float yfov = rfov * size.x / size.y;

        campitch = clamp(campitch - delta.y * yfov, radians(-60.f), radians(60.f));
        camyaw -= delta.x * rfov;

        camera.pose.rot = angleAxis(camyaw, vec3{0, 1, 0}) * angleAxis(campitch, vec3{1, 0, 0});
    });
}

void PKG::StepCamera(Camera& camera, float dt) noexcept {
    using Input::Held;
    vec3 delta{0};

    if (Held('S')) delta += vec3{0,0,1};
    if (Held('W')) delta -= vec3{0,0,1};
    if (Held('D')) delta += vec3{1,0,0};
    if (Held('A')) delta -= vec3{1,0,0};
    if (Held('E')) delta += vec3{0,1,0};
    if (Held('Q')) delta -= vec3{0,1,0};

    camera.pose += localspace(delta * dt, camera.pose.rot);
};
