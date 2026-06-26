#pragma once
#include <PKCore/vector.hpp>
#include <PKCore/string.hpp>
#include <PKAlias/math.hpp>

namespace pk {
    class Texture {
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif

        u32 id{0};
        void use(u32 layout) const;
        public:
            Texture() = default;
            Texture(strview path);
    };
}