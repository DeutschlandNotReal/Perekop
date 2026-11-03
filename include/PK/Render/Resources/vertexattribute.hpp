#pragma once
#include <PK/Render/Resources/buffer.hpp>
#include <cstddef>

namespace pk::Render {
    class VertexArray {
        friend struct Renderer;
        friend class Buffer;

        uint index{0};

        public:
            struct Builder {
                uint index{0};
                uint binding{0};
                size_t offset{0};

                Builder& SetBD(uint Binding, uint Divisor) noexcept;
                Builder& Float(int n = 1) noexcept;
                Builder& Uint() noexcept;
                Builder& Int() noexcept;
                Builder& Matrix4() noexcept;
            };

            static VertexArray generate() noexcept;

            void Bind() noexcept;
            void BindBuffer(uint Binding, const Buffer&, BufferTarget Target, size_t stride) noexcept;
    };
}