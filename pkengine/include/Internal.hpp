#pragma once
#include <Perekop.hpp>

class GLFWwindow;
namespace Perekop {
    inline const char *preamble_v{nullptr}, *preamble_f{nullptr};
    inline GLFWwindow* glfw_window{nullptr};
    inline pk::Array<glm::mat4> transforms{50};
    void draw();
    namespace init {
        extern void listeners();
        extern void render();
    }
    inline bool resized = false;

    inline uint gui_VAO{0}, gui_VBO{0}, gui_IBO{0}, gui_PROG{0};
}