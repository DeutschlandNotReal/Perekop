#pragma once
#include <PK/Physics/shape.hpp>
#include <PK/Physics/body.hpp>
#include <PK/Physics/AABB.hpp>

namespace pk::physics {
    struct overlapResult { 
        vec3 axis{0.f}; 
        float overlap{0.f}; 
        operator vec3() const noexcept { return axis * overlap; }
    };
    
    overlapResult getOverlap(const body&, const body&, AABB, AABB) noexcept;
};