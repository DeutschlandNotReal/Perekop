#pragma once
#include <PK/Core/span.hpp>

namespace pk {
    enum class bufferTar {
        element = 0x8893,
        array   = 0x8892
    };

    enum class bufferUse {
        stream  = 0x88E0,
        staticUsage = 0x88E4,
        dynamic = 0x88E8
    };

    class buffer {
        friend struct Renderer;

        public:
            uint index{0};

            buffer() noexcept = default;
            static buffer generate() noexcept;

            void upload(bufferTar, bufferUse, span<> data) noexcept;
            void destroy() noexcept;

            explicit operator bool() const noexcept { return index; }
            bool operator!() const noexcept { return !index; }

            buffer(buffer&&) noexcept;
            buffer& operator=(buffer&&) noexcept;
    };
}