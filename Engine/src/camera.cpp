#include <PK/Render/camera.hpp>
#include <PK/Render/window.hpp>
using namespace pk;

mat4 camera::view() const noexcept { return pose.inverse(); }

mat4 camera::projection() const noexcept {
    float rdepth = -1.f / ( max - min );
    float cotfov = 1.f / tfov;
    vec2 size = mainWindow.size();
    return {
        {cotfov * size.y / size.x, 0, 0, 0},
        {0, cotfov, 0, 0},
        {0, 0, (min + max) * rdepth, -1},
        {0, 0, (2 * min * max) * rdepth, 0}
    };
}

vec3 pk::worldspace(vec3 v, const camera& cam) noexcept {
    float ztan = v.z * cam.tanFov();
    vec2 size = mainWindow.size();

    v.x = (1.f - 2.f * v.x) * ztan * size.x / size.y;
    v.y = (1.f - 2.f * v.y) * ztan;

    return worldspace(-v, cam.pose);
}

vec3 pk::localspace(vec3 v, const camera& cam) noexcept {
    v = localspace(v, cam.pose);
    float cotinvz = .5f / (v.z * -cam.tanFov());
    vec2 size = mainWindow.size();

    v.x = .5f + v.x * cotinvz * size.y / size.x;
    v.y = .5f + v.y * cotinvz;

    return -v;
}