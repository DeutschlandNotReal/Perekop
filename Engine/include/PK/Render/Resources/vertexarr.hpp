#pragma once
#include <PK/Render/Resources/buffer.hpp>
#include <cstddef>

namespace pk {
    class VertexArr : public glResource<VertexArr> {
        friend struct Renderer;
        friend Buffer;

        struct Builder {
            uint index{0};
            uint binding{0};
            size_t offset{0};

            Builder& setBD(uint bind, uint div) noexcept;

            Builder&  vec(int d=1) noexcept;
            Builder& ivec(int d=1) noexcept;
            Builder& uvec(int d=1) noexcept;
            Builder&  mat(int d)   noexcept;

            void operator()() noexcept { VertexArr().bind(); };
        };

        public:
            void bind() noexcept;
            Builder config() { bind(); return {}; }

            void bindBuffer(uint bind, const Buffer&, BufferTar target, size_t stride) noexcept;
    };
}