#pragma once
#include <PK/Math/pose.hpp>

namespace pk {
    class Model {
        public:
            uint meshid{0};
            pose pose{};
            vec3 scale{1};
    };
} 