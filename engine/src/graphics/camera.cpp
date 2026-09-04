#include <PK/camera.hpp>
#include <PK/window.hpp>
using namespace pk;
using namespace Perekop;

mat4 Camera::view() const noexcept { return pose.inverse(); }

mat4 Camera::proj() const noexcept {
    float rdepth = -1.f / ( max - min );
    float cotfov = 1.f / tfov;
    vec2 size = Window::Size();
    return {
        {cotfov * size.y / size.x, 0, 0, 0},
        {0, cotfov, 0, 0},
        {0, 0, (min + max) * rdepth, -1},
        {0, 0, (2 * min * max) * rdepth, 0}
    };
}

vec3 pk::worldspace(vec3 v, const Camera& cam) noexcept {
    float ztan = v.z * cam.tanfov();
    vec2 size = Window::Size();

    float cx = (2 * v.x - 1) * ztan * size.x / size.y;
    float cy = (2 * v.y - 1) * ztan;

    return worldspace({cx, cy, -v.z}, cam.pose);
}

vec3 pk::localspace(vec3 v, const Camera& cam) noexcept {
    v = localspace(v, cam.pose);
    float cotinvz = -1.f / (v.z * -cam.tanfov());
    vec2 size = Window::Size();

    float x = v.x * cotinvz * size.y / size.x;
    float y = v.y * cotinvz;

    return {
        (x + 1) * .5f,
        (y + 1) * .5f,
        -v.z
    };
}