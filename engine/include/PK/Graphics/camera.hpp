#pragma once
#include <PK/pch.hpp>

namespace pk {
    struct Camera {
        f32 fmin{.1f}, fmax{200.f}, fov{70.f};
        
        vec4 pos;
        quat rot;
    };
}