#pragma once
#include <PK/Render/Primitive/texture.hpp>
#include <PK/Render/Primitive/shader.hpp>
#include <PK/Container/array.hpp>

namespace pk {
    namespace Render { struct Renderer; }
    class Material {
        friend Render::Renderer;
        
        public:
            array<Render::Texture, 1> textures;
            Render::Shader shader;    
    };
}