#pragma once
#include <PK/pose.hpp>

namespace pk {
    struct Model {
        pose pose; vec3 scale{1};
        unsigned short id{0}, mesh_id{0}, body_id{0};

        vec4 metadata; // shader metadata
    };
} 