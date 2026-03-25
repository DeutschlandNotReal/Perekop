#pragma once
#include "events.hpp"
#include "mesh.hpp"
#include <string>

#define exc extern const
namespace pk::engine {
    extern MeshRenderer mesh_renderer;
    void init();

    namespace window {
        exc EventPort<double>& step;
        exc EventPort<glm::vec2>& resized;
        exc EventPort<bool>& began;
        exc EventPort<bool>& ended;
        exc void close();
        exc void resize(glm::vec2 new_size);
        exc void rename(std::string new_title);
    }

    namespace input {
        exc glm::vec2 mouse_xy();

        exc EventPort<glm::vec2>& mouse_moved; // parameter is delta
        exc EventPort<int>& key_pressed;
        exc EventPort<int>& key_released;
        exc EventPort<int>& mouse_clicked;
        exc EventPort<int>& mouse_released; 
    }
}
#undef exc