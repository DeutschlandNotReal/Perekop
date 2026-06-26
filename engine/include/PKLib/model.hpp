#pragma once
#include <PKCore/slot_map.hpp>
#include <PKLib/pose.hpp>
#include <PKAlias/math.hpp>

namespace pk {
    class Model {
        friend slot_map<Model>;
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif

        public:
            u16 id{0}, mesh{0}, body{0};
            Pose pose;
            vec4 metadata;
    };
}