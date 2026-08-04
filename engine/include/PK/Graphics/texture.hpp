#pragma once
#include <PK/Core/string.hpp>

namespace pk {
    class Texture {
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif
        u32 txtid;
        public:
            void use(u32 layout) const noexcept;
            ~Texture();
            Texture(strview path);
            Texture(Texture&& b) = default; 
            Texture& operator=(Texture&&) = default;
    };
}