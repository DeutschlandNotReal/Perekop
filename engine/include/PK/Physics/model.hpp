#pragma once
#include <PK/Core/set.hpp>
#include <PK/Physics/pose.hpp>
#include <glm/vec4.hpp>

namespace pk {
    class Model {
        friend set<Model>;
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif

        public:
            u16 id{0}, mesh{0}, body{0};
            Pose pose;
            glm::vec4 metadata;
    };
}