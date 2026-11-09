#pragma once
#include <PK/Render/Resources/texture.hpp>
#include <PK/Render/Resources/shader.hpp>
#include <PK/Core/array.hpp>

namespace pk {
    struct renderer;

    class Material {
        friend renderer;
        
        public:
            array<texture, 1> textures;
            shader shader;    
    };
}