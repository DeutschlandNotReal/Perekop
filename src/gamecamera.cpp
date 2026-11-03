#include <PK/Input/mouse.hpp>
#include <PK/Input/keyboard.hpp>
#include <PK/Render/window.hpp>
#include <PK/Systems/gamecamera.hpp>
#include <PK/Physics/body.hpp>

using namespace pk;
using namespace Perekop;
using namespace Perekop::Input;

constexpr float maxpitch{radians(85.f)};
void PKG::GameCamera::Init() noexcept{
    rel = pose;
    vec3 euler = rel.EulerAngles();
    pitch = euler.x; yaw = euler.y;

    Mouse::BindToPress([](Mouse::Button button){
        if (button == Mouse::right) Mouse::Lock();
    });

    Mouse::BindToRelease([](Mouse::Button button){
        if (button == Mouse::right) Mouse::Unlock();
    });

    Mouse::BindToScroll([&](auto x){
        rel += worldspace(vec3{0, 0, -x}, rel.rot);
    });

    Mouse::BindToMove([&](vec2 delta){
        if (!Mouse::Held(Mouse::right)) return;

        vec2 size = Window::Size();
        Mouse::SetPosition(Mouse::Position() - delta);

        delta /= size;
        float rfov = radians(Fov());
        float yfov = rfov * size.x / size.y;

        pitch = clamp(pitch - delta.y * yfov, -maxpitch, maxpitch);
        yaw -= delta.x * rfov;
    });
}

void PKG::GameCamera::Step(float dt) noexcept {
    using Input::Held;
    vec3 delta{0};

    if (Held('S')) delta += vec3{0,0,1};
    if (Held('W')) delta -= vec3{0,0,1};
    if (Held('D')) delta += vec3{1,0,0};
    if (Held('A')) delta -= vec3{1,0,0};
    if (Held('E')) delta += vec3{0,1,0};
    if (Held('Q')) delta -= vec3{0,1,0};

    pk::pose target;
    bool hasTarget = false;
    if (fid) switch (ftype) {
        case FollowType::model:
            if (World::models.valid(fid)) {
                target = World::models[fid].pose;
                hasTarget = true;
            }
            break;
        case FollowType::body:
            if (World::bodies.valid(fid)) {
                target = World::bodies[fid].pose;
                hasTarget = true;
            }
            break;
        case FollowType::none:
            break;
    }

    if (type == Type::direct || !hasTarget)
        rel += worldspace(delta * dt * 15.f, pose.rot);
    else
        rel += worldspace(delta * dt * 15.f, rel.rot);

    vec3 worldPos = hasTarget && type != Type::direct
        ? worldspace(rel.pos, target)
        : rel.pos;
    vec3 g = Physics::GetGravity(worldPos);
    vec3 up = dot(g, g) != 0 ? normalize(-g) : vec3{0, 1, 0};
    if (hasTarget && type != Type::direct)
        up = localspace(up, target.rot);

    rel = pose::FromEuler(rel.pos, {pitch, yaw, 0});
    rel = pose::LookAlong(rel.pos, rel.Forward(), up);
    rel.rot = normalize(rel.rot);

    pk::pose desired = hasTarget && type != Type::direct
        ? worldspace(rel, target)
        : rel;

    if (type == Type::follow && hasTarget)
        pose = pose::Slerp(pose, desired, 1.f - exp(-8.f * dt));
    else
        pose = desired;
};

void PKG::GameCamera::Target(const entry<Model> tar) noexcept {
    ftype = FollowType::model;
    fid = tar.id;
    if (type != Type::direct) {
        rel = localspace(pose, World::models[tar.id].pose);
        vec3 euler = rel.EulerAngles();
        pitch = euler.x; yaw = euler.y;
    }
}

void PKG::GameCamera::Target(const entry<Body> tar) noexcept {
    ftype = FollowType::body;
    fid = tar.id;
    if (type != Type::direct) {
        rel = localspace(pose, World::bodies[tar.id].pose);
        vec3 euler = rel.EulerAngles();
        pitch = euler.x; yaw = euler.y;
    }
}

void PKG::GameCamera::Target() noexcept {
    ftype = FollowType::none;
    fid = 0;
    rel = pose;
    vec3 euler = rel.EulerAngles();
    pitch = euler.x; yaw = euler.y;
}