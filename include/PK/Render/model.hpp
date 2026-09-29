#pragma once
#include <PK/Physics/pose.hpp>

namespace pk {
    struct Model {
        pose pose; 
        vec3 scale{1};
    };
} 