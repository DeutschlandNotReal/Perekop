#pragma once
#include <filesystem>

namespace pk {
    using path = std::filesystem::path;

    class Texture {
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif
        
        unsigned txtid;
        public:
            void use(unsigned layout) const noexcept;
            ~Texture();
            Texture(const path& path);
            Texture(Texture&& b) = default; 
            Texture& operator=(Texture&&) = default;
    };
}