#pragma once
#include <PK/Render/Resources/buffer.hpp>
#include <cstddef>

namespace pk {
    class vtxAttribute {
        friend struct renderer;
        friend class buffer;

        uint index{0};

        public:
            struct Builder {
                uint index{0};
                uint binding{0};
                size_t offset{0};

                Builder& setBD(uint bind, uint div) noexcept;

                Builder&  vec(int d=1) noexcept;
                Builder& ivec(int d=1) noexcept;
                Builder& uvec(int d=1) noexcept;
                Builder&  mat(int d)   noexcept;
            };

            static vtxAttribute generate() noexcept;

            void bind() noexcept;
            void bindBuffer(uint Binding, const buffer&, bufferTar target, size_t stride) noexcept;
    };
}