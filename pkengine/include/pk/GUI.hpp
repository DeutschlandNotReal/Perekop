#pragma once
#include <pkutil/Event.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace pk {
    struct GUIObject {
        bool intersect(const glm::vec2& p) const {
            return !(p.x < pos.x || p.y < pos.y || p.x > pos.x + size.x || p.y > pos.y + size.y);
        }

        uint colour;
        glm::vec2 size{0}, pos{0};
        float Z{0};
        int id{0};
        bool entered{false};
    };
}