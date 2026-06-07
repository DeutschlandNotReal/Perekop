#pragma once
#include <PKLib/Math.hpp>
#include <PKLib/Event.hpp>

namespace pk {
    struct GUIObject {
        bool intersect(const vec2& p) const {
            return !(p.x < pos.x || p.y < pos.y || p.x > pos.x + size.x || p.y > pos.y + size.y);
        }

        vec4 col;
        vec2 size{0}, pos{0};
        float Z{0};
        int id{0};
        bool entered{false};
    };
}