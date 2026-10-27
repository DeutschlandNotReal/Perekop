#pragma once
#include <PK/Math/pose.hpp>

namespace pk::Physics {
    enum class ShapeType {
        Box, Ball
    };

    struct Shape {
        ShapeType type;
        pose pose;
        union {
            float r;
            vec3 ext;
        };

        Shape(struct pose P, float r) noexcept : type{ShapeType::Ball}, pose{P}, r{r} {}
        Shape(struct pose P, vec3 exts) noexcept : type{ShapeType::Box}, pose{P}, ext{exts} {}
    };
}
