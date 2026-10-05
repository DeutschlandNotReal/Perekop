#pragma once
#include <PK/Math/pose.hpp>

namespace pk {
    class Model {
        public:
            unsigned meshid{0}, bodyid{0};
            pose pose; 
            vec3 scale{1};
    };
} 