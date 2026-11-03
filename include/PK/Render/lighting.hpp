#pragma once
#include <PK/Render/camera.hpp>
#include <PK/Render/renderer.hpp>

namespace pk::Lighting {
    inline pose Light;
    inline Render::Renderer Renderer;
    inline Render::Texture Material;

    namespace ShaderData {
        inline mat4 CameraMatrix;
        inline mat4 LightMatrix;
    }
}