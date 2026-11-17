#include <PK/Render/window.hpp>
#include <PK/Physics/Body.hpp>
#include <PK/tick.hpp>

#include "./movecam.hpp"
using namespace pk;

constexpr float maxpitch{radians(85.f)};

void PKG::MovableCamera::connect() noexcept{
    rel = pose;
    vec3 euler = rel.eulerAngles();
    pitch = euler.x; yaw = euler.y;

    Window::main.onMouseDown.bind([](MouseKey button){
        if (button == MouseKey::right) Window::main.setMouseLocked(true);
    });


    Window::main.onMouseUp.bind([](MouseKey button){
        if (button == MouseKey::right) Window::main.setMouseLocked(false);
    }); 

    Window::main.onMouseScroll.bind([&](auto x){
        rel += worldspace(vec3{0, 0, -x}, rel.rot);
    });

    Window::main.onMouseMove.bind([&](vec2 delta){
        if (!Window::main.held(MouseKey::right)) return;

        vec2 size = Window::main.size();
        if (size.x <= 0.f || size.y <= 0.f) return;

        delta /= size;
        float rfov = radians(fov());
        float yfov = rfov * size.x / size.y;

        pitch = clamp(pitch - delta.y * yfov, -maxpitch, maxpitch);
        yaw -= delta.x * rfov;
    });

}

void PKG::MovableCamera::step(float dt) noexcept {
    vec3 delta{0};

    if (Window::main.held('S')) delta += vec3{0,0,1};
    if (Window::main.held('W')) delta -= vec3{0,0,1};
    if (Window::main.held('D')) delta += vec3{1,0,0};
    if (Window::main.held('A')) delta -= vec3{1,0,0};
    if (Window::main.held('E')) delta += vec3{0,1,0};
    if (Window::main.held('Q')) delta -= vec3{0,1,0};

    pk::pose target;
    bool hasTarget = false;

    if (follow) {
        Model* model = follow.get();
        if (!model) { 
            follow = nullid; 
        } else {
            target = model->pose;
            hasTarget = true;
        }
    }

    if (type == Type::direct || !hasTarget)
        rel += worldspace(delta * dt * 15.f, pose.rot);
    else
        rel += worldspace(delta * dt * 15.f, rel.rot);

    bool followsTarget = hasTarget && type != Type::direct;
    vec3 worldPos = followsTarget
        ? worldspace(rel.pos, target)
        : rel.pos;
    vec3 up{0, 1, 0};
    if (followsTarget) {
        vec3 g = getWeight(worldPos);
        if (dot(g, g) != 0) up = normalize(-g);
        up = localspace(up, target.rot);
    }

    rel = pose::fromEuler(rel.pos, {pitch, yaw, 0});
    rel = pose::lookAlong(rel.pos, rel.forward(), up);
    rel.rot = normalize(rel.rot);

    pk::pose desired = followsTarget
        ? worldspace(rel, target)
        : rel;

    if (type == Type::follow && hasTarget)
        pose = pose::slerp(pose, desired, 1.f - exp(-8.f * dt));
    else
        pose = desired;
};

void PKG::MovableCamera::target(ModelHandle &tar) noexcept {
    follow = tar.getID();
    if (type != Type::direct) {
        rel = localspace(pose, tar.pose);
        vec3 euler = rel.eulerAngles();
        pitch = euler.x; yaw = euler.y;
    }
}

void PKG::MovableCamera::target() noexcept {
    follow = nullid;
    rel = pose;
    vec3 euler = rel.eulerAngles();
    pitch = euler.x; yaw = euler.y;
}