#pragma once
#include <PK/pose.hpp>

namespace pk {
    struct Model {
        pose pose;
        unsigned short id, mesh_id{0}, body_id{0};

        vec4 metadata; // shader metadata
    };
} 