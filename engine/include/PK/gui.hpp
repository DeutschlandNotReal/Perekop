#pragma once
#include <PK/pose.hpp>
#include <PK/shader.hpp>

namespace pk::gui {
    enum class type: char {
        basic = 0,
        text  = 1,
        image = 2
    };

    struct instance {
        vec2 pos{0}, size{0};
        bool entered{false};
        float Z{0};

        bool intersects(vec2 point) const noexcept {
            return !(point.x < pos.x || point.y < pos.y || point.x > pos.x + size.x || point.y > pos.y + size.y);
        }

        type type{type::basic};
        
        union {
            Texture image;

        };

        ~instance() {
            
        };
    };

    extern instance* top_gui;
}