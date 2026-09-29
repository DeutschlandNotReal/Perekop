#pragma once
#include <PK/Render/Primitive/buffer.hpp>
#include <cstddef>

namespace pk::Render {
    class VertexArray {
        friend struct Renderer;
        friend class Buffer;

        unsigned index{0};

        public:
            struct Builder {
                unsigned index{0};
                unsigned binding{0};
                size_t offset{0};

                Builder& SetBD(unsigned Binding, unsigned Divisor) noexcept;
                Builder& Float(int n = 1) noexcept;
                Builder& Uint() noexcept;
                Builder& Int() noexcept;
                Builder& Matrix4() noexcept;
            };

            static VertexArray generate() noexcept;

            void Bind() noexcept;
            void BindBuffer(unsigned Binding, const Buffer&, BufferTarget Target, size_t stride) noexcept;
    };
}