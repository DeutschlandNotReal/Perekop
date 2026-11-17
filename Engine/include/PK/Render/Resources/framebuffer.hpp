#pragma once
#include "./glresource.hpp"
#include <PK/Render/Resources/texture.hpp>

namespace pk {
    class Renderbuffer : public glResource<Renderbuffer> {
        public:
            void bind() const noexcept;
 
            void allocate(uint width, uint height, uint format);
    };

     class Framebuffer : public glResource<Framebuffer> {
        public:
            void bind() const noexcept;
   
            void attach(Texture& texture, uint slot = 0);
            void attach(Renderbuffer& renderbuffer);

            bool complete() const noexcept;
    };
}
