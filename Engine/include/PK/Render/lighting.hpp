#pragma once

#include <PK/Render/camera.hpp>
#include <PK/Render/renderer.hpp>

namespace pk::lighting {
    inline pose light;
    inline renderer renderer;
    inline texture material;

    namespace shaderData {
        inline mat4 cameraMatrix;
        inline mat4 lightMatrix;
    }
}