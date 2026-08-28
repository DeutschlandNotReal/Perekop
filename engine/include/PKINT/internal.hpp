#pragma once

#ifdef PK_INTERNAL
namespace Perekop { void render(bool); }

#include <PK/callbacks.hpp>
#include <PK/userinput.hpp>
#include <PK/window.hpp>
#include <PK/world.hpp>

class GLFWwindow;
namespace pk {
    struct GuiData { 
        float Z; 
        vec2 p, s; 
        vec4 RGBA;
    };
};

namespace Perekop {
    inline GLFWwindow* glfw_window{nullptr};

    namespace cache {
        inline pk::vector<pk::GuiData> gui;
    }

    void init_render();
    void init_window();

    void step_window();
    void step_physics(float dt);

    extern void query_gui();
}
#endif