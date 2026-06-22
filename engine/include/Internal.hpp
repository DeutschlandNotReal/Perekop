#pragma once
#ifdef PK_ENGINE_SRC
namespace Perekop { void render(bool recollect); }
#define PK_INTERNAL
#include <Perekop/callbacks.hpp>
#include <Perekop/gui.hpp>
#include <Perekop/input.hpp>
#include <Perekop/window.hpp>
#include <Perekop/world.hpp>

class GLFWwindow;
namespace pk {
    struct GuiData { 
        float Z; 
        glm::vec2 p, s; 
        glm::vec4 RGBA;
    };

    struct ModelData { 
        glm::mat4 transform; 
        glm::vec4 metadata; 
    };
};

namespace Perekop {
    inline pk::string preamble_v{}, preamble_f{};
    inline GLFWwindow* glfw_window{nullptr};

    namespace cache {
        inline pk::vector<pk::vector<pk::ModelData>> T;
        inline pk::vector<pk::GuiData> gui;
    }

    void init_render();
    void init_window();

    void step_window();
    void step_physics(float dt);

    inline uint32_t
        mesh_VAO{0},
        gui_VAO{0}, 
        gui_VBO{0}, 
        gui_IBO{0}, 
        gui_PROG{0};

    extern void query_gui();

    extern glm::mat3 to_inertia(glm::vec3 localpoint);
}

#endif