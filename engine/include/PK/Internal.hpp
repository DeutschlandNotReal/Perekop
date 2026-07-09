#pragma once
#ifdef PK_INTERNAL
namespace Perekop { void render(bool recollect); }
#include <PK/pch.hpp>

#include <PK/Interface/callbacks.hpp>
#include <PK/Interface/input.hpp>
#include <PK/Interface/window.hpp>
#include <PK/Interface/world.hpp>

class GLFWwindow;
namespace pk {
    struct GuiData { 
        float Z; 
        vec2 p, s; 
        vec4 RGBA;
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
        inline pk::Vec<pk::Vec<pk::ModelData>> T;
        inline pk::Vec<pk::GuiData> gui;
    }

    void init_render();
    void init_window();

    void step_window();
    void step_physics(float dt);

    inline u32
        mesh_VAO{0},
        gui_VAO{0}, 
        gui_VBO{0}, 
        gui_IBO{0}, 
        gui_PROG{0};

    extern void query_gui();

    extern glm::mat3 to_inertia(glm::vec3 localpoint);
}
#endif