#pragma once
#include <PK/Render/Resources/buffer.hpp>
#include <PK/Render/Resources/framebuffer.hpp>
#include <PK/Render/Resources/shader.hpp>
#include <PK/Render/Resources/texture.hpp>
#include <PK/Render/Resources/vertexattribute.hpp>

namespace pk::Render {
    enum class RasterMode {
        Fill  = 0x1B02,
        Line  = 0x1B01,
        Point = 0x1B00
    };

    enum class PrimitiveMode {
        Points = 0x0000,
        Lines  = 0x0001,
        Trigs  = 0x0004
    };

    enum class ClearTarget {
        Depth  = 0x0100,
        Colour = 0x4000
    };

    enum class DrawTarget {
        None  = 0,
        Front = 0x0404,
        Back  = 0x0405
    };
    
    struct Renderer {
        void DrawElems(int indices, PrimitiveMode mode = PrimitiveMode::Trigs) noexcept;
        void DrawArrays(int vertices, PrimitiveMode mode = PrimitiveMode::Trigs) noexcept;
        void iDrawElems(int indices, int entities, PrimitiveMode mode = PrimitiveMode::Trigs) noexcept;
        void iDrawArrays(int vertices, int entities, PrimitiveMode mode = PrimitiveMode::Trigs) noexcept;

        void SetMode(RasterMode mode) noexcept;
        void Target(const Framebuffer&, DrawTarget target = DrawTarget::None) noexcept;
        void Fill(vec3 colour) noexcept;
        void Clear(int flags) noexcept;

        void Viewport() noexcept;
        void Viewport(int width, int height) noexcept;
        void Viewport(int x, int y, int width, int height) noexcept;

        void Swap() noexcept;
    };
}