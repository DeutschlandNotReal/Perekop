#pragma once

#include <PK/Math/pose.hpp>
#include <PK/Core/setID.hpp>

namespace pk {
    class model {
        public:
            setID meshid;
            pose pose;
            vec3 scale{1};
            vec3 tint{1};
    };
}