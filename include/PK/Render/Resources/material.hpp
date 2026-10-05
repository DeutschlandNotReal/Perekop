#pragma once
#include <PK/Render/Resources/texture.hpp>
#include <PK/Render/Resources/shader.hpp>
#include <PK/Core/array.hpp>

namespace pk {
    namespace Render { struct Renderer; }
    class Material {
        friend Render::Renderer;
        
        public:
            array<Render::Texture, 1> textures;
            Render::Shader shader;    
    };
}