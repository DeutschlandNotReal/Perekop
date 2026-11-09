#include <PK/Render/window.hpp>
#include <PK/Physics/body.hpp>
#include <PK/tick.hpp>

#include "./gamecam.hpp"
#include "./functions.hpp"

using namespace pk;


namespace {
    PKG::GameCam gcamera;
}

constexpr float maxpitch{radians(85.f)};

void PKG::GameCam::init() noexcept{
    rel = pose;
    vec3 euler = rel.eulerAngles();
    pitch = euler.x; yaw = euler.y;

    mainWindow.input.bindMPress([](mouseButton button){
        if (button == mouseButton::right) mainWindow.input.lockMouse();
    });

    mainWindow.input.bindMRelease([](mouseButton button){
        if (button == mouseButton::right) mainWindow.input.unlockMouse();
    });

    mainWindow.input.bindScroll([&](auto x){
        rel += worldspace(vec3{0, 0, -x}, rel.rot);
    });

    mainWindow.input.bindMove([&](vec2 delta){
        if (!mainWindow.input.mouseHeld(mouseButton::right)) return;

        vec2 size = mainWindow.size();
        if (size.x <= 0.f || size.y <= 0.f) return;

        delta /= size;
        float rfov = radians(fov());
        float yfov = rfov * size.x / size.y;

        pitch = clamp(pitch - delta.y * yfov, -maxpitch, maxpitch);
        yaw -= delta.x * rfov;
    });
}

void PKG::GameCam::step(float dt) noexcept {
    vec3 delta{0};

    if (mainWindow.input.keyHeld('S')) delta += vec3{0,0,1};
    if (mainWindow.input.keyHeld('W')) delta -= vec3{0,0,1};
    if (mainWindow.input.keyHeld('D')) delta += vec3{1,0,0};
    if (mainWindow.input.keyHeld('A')) delta -= vec3{1,0,0};
    if (mainWindow.input.keyHeld('E')) delta += vec3{0,1,0};
    if (mainWindow.input.keyHeld('Q')) delta -= vec3{0,1,0};

    pk::pose target;
    bool hasTarget = false;
    if (fid) switch (ftype) {
        case FollowType::model:
            if (world::models.valid(fid)) {
                target = world::models[fid].pose;
                hasTarget = true;
            }
            break;
        case FollowType::body:
            if (world::bodies.valid(fid)) {
                target = world::bodies[fid].pose;
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

void PKG::GameCam::target(entry<model> tar) noexcept {
    ftype = FollowType::model;
    fid = tar.id;
    if (type != Type::direct) {
        rel = localspace(pose, world::models[tar.id].pose);
        vec3 euler = rel.eulerAngles();
        pitch = euler.x; yaw = euler.y;
    }
}

void PKG::GameCam::target(entry<body> tar) noexcept {
    ftype = FollowType::body;
    fid = tar.id;
    if (type != Type::direct) {
        rel = localspace(pose, world::bodies[tar.id].pose);
        vec3 euler = rel.eulerAngles();
        pitch = euler.x; yaw = euler.y;
    }
}

void PKG::GameCam::target() noexcept {
    ftype = FollowType::none;
    fid = 0;
    rel = pose;
    vec3 euler = rel.eulerAngles();
    pitch = euler.x; yaw = euler.y;
}

void PKG::onInit::camera() {
    printf("Gamecam init\n");
    mainWindow.mainCamera = &gcamera;
    gcamera.pose = pose::lookAt({0, 8, 25}, {0, 0, 0});
    gcamera.init();
    printf("Gamecam init end\n");
}

void PKG::onStep::camera(float dt) {
    printf("Gamecam step\n");
    gcamera.step(dt);
    printf("Gamecam step end\n");
}