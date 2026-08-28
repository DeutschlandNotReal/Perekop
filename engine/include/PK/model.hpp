#pragma once
#include <PK/pose.hpp>

namespace pk {
    struct Model {
        pose pose; 
        vec3 scale{1};
        vec4 metadata; // shader metadata
    };
} 