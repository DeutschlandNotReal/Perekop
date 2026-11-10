#pragma once
#include <PK/Physics/shape.hpp>
#include <PK/Physics/Body.hpp>
#include <PK/Physics/AABB.hpp>

namespace pk::physics {
    struct overlapResult { 
        vec3 axis{0.f}; 
        float overlap{0.f}; 
        operator vec3() const noexcept { return axis * overlap; }
    };
    
    overlapResult getOverlap(const Body&, const Body&, AABB, AABB) noexcept;
};