#pragma once
#include <PK/Math/transform.hpp>

namespace pk {
    class Model {
        public:
            transform t;
            u16 id{0}, mesh{0}, body{0};

            vec4 metadata;
    };
} 