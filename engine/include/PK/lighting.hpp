#pragma once
#include <PK/camera.hpp>
#include <PK/shader.hpp>

namespace pk::Lighting {
    inline Camera Camera;
    inline pose   Light;
    inline Render Renderer;
    
    namespace Shadow {
        extern Shader Shader;
        extern Framebuffer Buffer;
        inline Texture Texture;
    }

    namespace ShaderData {
        inline mat4 CameraMatrix;
        inline mat4 LightMatrix;
    }
}