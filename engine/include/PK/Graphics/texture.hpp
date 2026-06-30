#pragma once
#include <PK/Core/vector.hpp>
#include <PK/Core/string.hpp>

namespace pk {
    class Texture {
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif

        u32 id{0};
        void use(u32 layout) const;
        public:
            Texture() = default;
            Texture(SView path);
    };
}