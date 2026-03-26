#pragma once
#include "events.hpp"
#include "mesh.hpp"
#include <string>

namespace pk::engine {
    extern MeshRenderer mesh_renderer;
    void init();

    namespace window {
        extern const EventPort<double>& step;
        extern const EventPort<glm::vec2>& resized;
        extern const EventPort<bool>& began;
        extern const EventPort<bool>& ended;
        extern void close();
        extern void resize(glm::vec2 new_size);
        extern void rename(std::string new_title);
        extern glm::vec2 get_size();
    }

    namespace input {
        extern glm::vec2 mouse_xy();

        extern EventPort<glm::vec2>& mouse_moved; // parameter is delta
        extern EventPort<int>& key_pressed;
        extern EventPort<int>& key_released;
        extern EventPort<int>& mouse_clicked;
        extern EventPort<int>& mouse_released; 
    }
}

#undef exc