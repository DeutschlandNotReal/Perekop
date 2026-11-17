#pragma once

#include <PK/Render/Resources/shader.hpp>
#include <PK/Render/Resources/texture.hpp>
#include <PK/Render/Resources/vertexarr.hpp>

namespace pk {
    enum class RasterMode {
        fill  = 0x1B02,
        line  = 0x1B01,
        point = 0x1B00
    };

    enum class PrimMode {
        points = 0x0000,
        lines  = 0x0001,
        trigs  = 0x0004
    };

    enum class ClearTar {
        depth  = 0x0100,
        colour = 0x4000
    };

    enum class DrawMode {
        element = 0,
        array   = 1
    };

    struct Renderer {
        void draw(
            DrawMode, 
            int n, 
            PrimMode mode = PrimMode::trigs
        ) noexcept;

        void drawInstanced(
            DrawMode, 
            int n, 
            int instances,
            PrimMode mode = PrimMode::trigs
        ) noexcept;

        void setMode(RasterMode mode) noexcept;
        void fill(vec3 colour) noexcept;
        void clear(int flags) noexcept;

        void viewport() noexcept;

        void swap() noexcept;
    };
}