#pragma once
#include <PK/Render/Resources/Texture.hpp>
#include <PK/Render/Resources/shader.hpp>
#include <PK/Core/array.hpp>

namespace pk {
    struct Renderer;

    class Material {
        friend Renderer;
        
        public:
            array<Texture, 1> textures;
            shader shader;    
    };
}