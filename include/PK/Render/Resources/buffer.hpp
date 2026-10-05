#pragma once
#include <PK/Core/span.hpp>

namespace pk::Render {
    enum class BufferTarget {
        Element = 0x8893,
        Array   = 0x8892
    };

    enum class BufferUsage {
        Stream  = 0x88E0,
        Static  = 0x88E4,
        Dynamic = 0x88E8
    };

    class Buffer {
        friend struct Renderer;
        friend class VertexArray;

        unsigned index{0};

        public:
            Buffer() noexcept = default;
            static Buffer Generate() noexcept;

            void Upload(BufferTarget, BufferUsage, span<void> data) noexcept;

            void Delete() noexcept;

            explicit operator bool() const noexcept { return index; }
            bool operator!() const noexcept { return !index; }

            Buffer(Buffer&&) noexcept;
            Buffer& operator=(Buffer&&) noexcept;
    };
}