#pragma once
#include <PK/Core/string.hpp>
#include <PK/Util/event.hpp>
#include <PK/Graphics/texture.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
using glm::vec2, glm::vec3;

namespace pk {
    enum class gui_type: char {
        basic = 0,
        text  = 1,
        image = 2
    };

    struct gui_instance {
        vec2 pos{0}, size{0};
        bool entered{false};
        f32 Z{0};
        bool is_intersecting(vec2 point) const {
            return !(point.x < pos.x || point.y < pos.y || point.x > pos.x + size.x || point.y > pos.y + size.y);
        }

        gui_type type{gui_type::basic};
        
        union {
            String text;
            Texture image;
            vec3 col;
        };

        ~gui_instance() {
            if (type == gui_type::text) text.~String();
        };
    };
}