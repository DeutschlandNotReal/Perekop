#include <PK/Render/lighting.hpp>
#include <PK/Render/Resources/shader.hpp>
#include <PK/tick.hpp>
#include <PK/world.hpp>

#include "./common.hpp"
#include "./functions.hpp"

using namespace pk;

void PKG::onInit::render() {
    lighting::light = pose::lookAt({50, 36, 22}, {0, 2, 0});

    
    PKG::material.shader = {
        {shaderType::Vertex, world::assets / "shaders/vert.glsl"},
        {shaderType::fragment, world::assets / "shaders/frag.glsl"}
    };

    PKG::material.textures[0] = lighting::material = world::assets/"images/test.jpg";
}