#pragma once

namespace Perekop { void render(bool recollect); }
#define PK_INTERNAL
#include <Perekop.hpp>

class GLFWwindow;
namespace pk {
    struct rawgui { float Z; glm::vec2 p, s; glm::vec4 RGBA; };
};

namespace Perekop {
    inline const char *preamble_v{nullptr}, *preamble_f{nullptr};
    inline GLFWwindow* glfw_window{nullptr};
    inline pk::Array<pk::Array<glm::mat4>> transforms;
    inline pk::Array<pk::rawgui> guidata;

    void init_render();
    void init_window();

    void step_window();
    void step_physics(float dt);

    inline uint
        mesh_VAO{0},
        gui_VAO{0}, 
        gui_VBO{0}, 
        gui_IBO{0}, 
        gui_PROG{0};

    extern void query_gui();
}

#undef PK_INTERNAL