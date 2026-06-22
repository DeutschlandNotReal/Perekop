#pragma once
#include <PKCore/set.hpp>
#include <PKLib/pose.hpp>
#include <PKLib/math.hpp>

namespace pk {
    class Model {
        friend set<Model>;
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif

        public:
            uint16_t id{0}, mesh{0}, body{0};
            Pose pose;
            vec4 metadata;
    };
}