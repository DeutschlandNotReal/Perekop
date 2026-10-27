#pragma once
#include <PK/Physics/shape.hpp>

namespace pk::Physics {
    struct OverlapResult { 
        vec3 axis{0.f}; 
        float overlap{0.f}; 
        operator vec3() const noexcept { return axis * overlap; }
    };
    
    OverlapResult GetOverlap(const Shape&, const Shape&) noexcept;
};