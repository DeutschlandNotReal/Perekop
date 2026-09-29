#pragma once

namespace pk::Render {
    struct Renderer;
    class Texture;

    class Framebuffer {
        friend Renderer;

        unsigned index{0};
        int w{0};
        int h{0};
        bool ColourAttached{false};

        public:
            Framebuffer() noexcept = default;
            Framebuffer(int width, int height) noexcept;

            void Bind() noexcept;
            void AttachColour(const Texture&) noexcept;
            void AttachDepth(const Texture&) noexcept;
    };
}