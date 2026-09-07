#pragma once
#include <PK/pose.hpp>
#include <PK/shader.hpp>

namespace pk::gui {
    enum class Type: char {
        Basic = 0,
        Text  = 1,
        Image = 2
    };

    struct Instance {
        vec2 pos{0}, size{0};
        bool entered{false};
        float Z{0};

        bool Intersects(vec2 point) const noexcept {
            return !(point.x < pos.x || point.y < pos.y || point.x > pos.x + size.x || point.y > pos.y + size.y);
        }

        Type type{Type::Basic};
        
        union {
            Texture image;

        };

        ~Instance() {
            
        };
    };

    extern Instance* top_gui;
}