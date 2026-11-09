#pragma once

#include <PK/Render/Resources/shader.hpp>
#include <PK/Render/Resources/texture.hpp>
#include <PK/Render/Resources/vertexattribute.hpp>

namespace pk {
    enum class rasterMode {
        fill  = 0x1B02,
        line  = 0x1B01,
        point = 0x1B00
    };

    enum class primitiveMode {
        points = 0x0000,
        lines  = 0x0001,
        trigs  = 0x0004
    };

    enum class clearTar {
        depth  = 0x0100,
        colour = 0x4000
    };

    struct renderer {
        void drawElems(int indices, primitiveMode mode = primitiveMode::trigs) noexcept;
        void drawArrays(int vertices, primitiveMode mode = primitiveMode::trigs) noexcept;
        void drawElemsInstanced(int indices, int entities, primitiveMode mode = primitiveMode::trigs) noexcept;
        void drawArraysInstanced(int vertices, int entities, primitiveMode mode = primitiveMode::trigs) noexcept;

        void setMode(rasterMode mode) noexcept;
        void fill(vec3 colour) noexcept;
        void clear(int flags) noexcept;

        void viewport() noexcept;

        void swap() noexcept;
    };
}