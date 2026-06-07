#pragma once
#ifdef PK_ENGINE_SRC
namespace Perekop { void render(bool recollect); }
#define PK_INTERNAL
#include <Perekop.hpp>

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
    inline const char *preamble_v{nullptr}, *preamble_f{nullptr};
    inline GLFWwindow* glfw_window{nullptr};

    namespace cache {
        inline pk::dynarray<pk::dynarray<pk::ModelData>> T;
        inline pk::dynarray<pk::GuiData> gui;
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

#undef PK_INTERNAL
#endif