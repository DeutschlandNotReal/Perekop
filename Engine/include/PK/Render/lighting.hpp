#pragma once

#include <PK/Render/Camera.hpp>
#include <PK/Render/Renderer.hpp>

namespace pk::lighting {
    inline pose light;
    inline Renderer Renderer;
    inline Texture material;

    namespace shaderData {
        inline mat4 cameraMatrix;
        inline mat4 lightMatrix;
    }
}