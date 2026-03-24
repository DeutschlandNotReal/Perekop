#pragma once
#include "events.hpp"
#include "mesh.hpp"
#include <string>

namespace pk::engine {
    extern MeshRenderer mesh_renderer;

    namespace window {
        extern const EventPort<float> step;
        extern const EventPort<glm::vec2> resized;
        extern const EventPort<bool> began;
        extern const EventPort<bool> ended;
        extern const void close();
        extern const void resize(glm::vec2 new_size);
        extern const void rename(std::string new_title);
    }

    namespace input {
        extern const glm::vec2 mouse_xy();

        extern const EventPort<glm::vec2> mouse_moved; // parameter is delta
        extern const EventPort<int> key_pressed;
        extern const EventPort<int> key_released;
        extern const EventPort<int> mouse_clicked;
        extern const EventPort<int> mouse_released; 
    }
}