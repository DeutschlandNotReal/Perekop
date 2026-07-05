#pragma once
#include <PK/Math/number.hpp>
#include <PK/Math/simd.hpp>
#include <PK/Math/functional.hpp>
#include <PK/Core/type.hpp>
#include <cmath>

namespace pk {
    // soon...
    struct vec2 { f32 x, y; };

    struct vec3 { f32 x, y, z; };

    struct vec4 : f32x4<vec4> {
        using f32x4<vec4>::f32x4;
    };

    struct quat: f32x4<quat> {
        using f32x4<quat>::f32x4;
    };
}