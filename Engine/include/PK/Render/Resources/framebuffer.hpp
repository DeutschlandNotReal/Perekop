#pragma once
#include <PK/Math/typedef.hpp>
#include <PK/Render/Resources/texture.hpp>


namespace pk {
    class Renderbuffer;

    class Framebuffer {
        uint index{0};

        public:
            Framebuffer() noexcept = default;

            uint getIndex() const noexcept { return index; }

            void generate();
            void destroy();

            void bind() const noexcept;
            static void unbind() noexcept;

            void attach(Texture& texture, uint slot = 0);
            void attach(Renderbuffer& renderbuffer);

            bool complete() const noexcept;
    };

    class Renderbuffer {
        uint index{0};

        public:
            uint getIndex() const noexcept { return index; }

            void generate();
            void destroy();

            void bind() const noexcept;
            static void unbind() noexcept;

            void allocate(
                uint width,
                uint height,
                uint format
            );
    };
}
