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

    struct Wirebox {
        mat3 rot;
        vec3 ext,
             pos;
        vec4 col;
    };

    struct Renderer {
        void drawElems(int indices, PrimMode mode = PrimMode::trigs) noexcept;
        void drawArrays(int vertices, PrimMode mode = PrimMode::trigs) noexcept;
        void drawElemsInstanced(int indices, int entities, PrimMode mode = PrimMode::trigs) noexcept;
        void drawArraysInstanced(int vertices, int entities, PrimMode mode = PrimMode::trigs) noexcept;

        void drawWireboxes(const span<Wirebox>) noexcept;

        void setMode(RasterMode mode) noexcept;
        void fill(vec3 colour) noexcept;
        void clear(int flags) noexcept;

        void viewport() noexcept;

        void swap() noexcept;
    };
}