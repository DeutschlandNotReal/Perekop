#pragma once
#include <Perekop.hpp>

class GLFWwindow;
namespace pk {
    struct rawgui { float Z; glm::vec2 p, s; glm::vec4 RGBA; };
};
namespace Perekop {
    inline const char *preamble_v{nullptr}, *preamble_f{nullptr};
    inline GLFWwindow* glfw_window{nullptr};
    inline pk::Array<glm::mat4> transforms{50};
    inline pk::Array<pk::rawgui> guidata;

    namespace init {
        extern void draw();
        extern void window();
    }

    namespace step {
        void draw();
        void window();
    }

    inline uint
        mesh_VAO{0},
        gui_VAO{0}, 
        gui_VBO{0}, 
        gui_IBO{0}, 
        gui_PROG{0};

    extern void get_gui_top();

    namespace Mouse { 
        inline glm::vec2 pos_v{0}; 
    }

    namespace Window { 
        inline glm::vec2 pos_v{0}, size_v{0}; 
        inline const char* title_v{nullptr};
    }
}