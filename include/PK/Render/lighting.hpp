#pragma once
#include <PK/Render/camera.hpp>
#include <PK/Render/renderer.hpp>

namespace pk::Lighting {
    inline Camera Camera;
    inline pose   Light;
    inline Render::Renderer Renderer;
    inline Render::Texture Material;
    
    namespace Shadow {
        inline Render::Shader Shader;
        inline Render::Framebuffer Buffer;
        inline Render::Texture Texture;
    }

    namespace ShaderData {
        inline mat4 CameraMatrix;
        inline mat4 LightMatrix;
    }
}