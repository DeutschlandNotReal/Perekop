#pragma once
#include <Perekop.hpp>

class GLFWwindow;
namespace Perekop {
    inline const char *preamble_v{nullptr}, *preamble_f{nullptr};
    inline GLFWwindow* glfw_window{nullptr};
    inline pk::Array<glm::mat3x4> transforms{50};
    void draw();
    void init_input_listeners();
}