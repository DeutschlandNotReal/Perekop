#pragma once
#include <PK/Math/pose.hpp>

namespace pk::Physics {
    struct Sphere { float r;   }; 
    struct Box    { vec3  ext; };

    bool Test(Sphere, Sphere, vec3) noexcept;
    bool TestOOB(Box, Box, pose) noexcept;
    bool TestAABB(Box, Box, vec3) noexcept;
    bool Test(Sphere, Box, vec3) noexcept;
};