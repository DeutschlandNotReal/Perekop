#pragma once
#include <PKCore/vector.hpp>
#include <PKCore/string.hpp>
#include <PKLib/math.hpp>

namespace pk {
    class Texture {
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif

        uint32_t id{0};
        void use(uint32_t layout) const;
        public:
            Texture() = default;
            Texture(strview path);
    };
}