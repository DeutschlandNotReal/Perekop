#pragma once
#include <Perekop.hpp>

class GLFWwindow;
namespace pk {
    struct rawgui { float Z; glm::vec2 p, s; glm::vec4 RGBA; };
};
namespace Perekop {
    inline const char *preamble_v{nullptr};
    inline GLFWwindow* glfw_window{nullptr};
    inline pk::Array<glm::mat4> transforms{50};
    inline pk::Array<pk::rawgui> guidata;
    void draw();
    namespace init {
        extern void listeners();
        extern void render();
    }
    inline bool resized = false;

    inline uint
        mesh_VAO{0},
        gui_VAO{0}, 
        gui_VBO{0}, 
        gui_IBO{0}, 
        gui_PROG{0};

    // tests if mouse is in gui when some mouse events happen
    // blocks event firing if test pass...
    extern bool gui_test_move();
    extern bool gui_test_down(int k);
    extern bool gui_test_up(int k);
    // extern bool gui_test_scroll(int d);
}