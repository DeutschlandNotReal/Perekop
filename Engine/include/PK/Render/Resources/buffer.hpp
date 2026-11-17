#pragma once
#include <PK/Core/span.hpp>
#include "./glresource.hpp"

namespace pk {
    enum class BufferTar {
        element = 0x8893,
        array   = 0x8892
    };

    enum class BufferUse {
        stream  = 0x88E0,
        staticUsage = 0x88E4,
        dynamic = 0x88E8
    };

    class Buffer : public glResource<Buffer> {
        friend struct Renderer;
   
        public:
            void upload(BufferTar, BufferUse, span<> data) noexcept;
    };
}