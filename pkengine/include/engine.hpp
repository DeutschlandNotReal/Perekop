#pragma once
#include "events.hpp"
#include "mesh.hpp"
#include <string>

namespace pk::engine {
    extern MeshRenderer mesh_renderer;

    namespace frame {
        extern EventPort<float> stepped;
        extern EventPort<glm::vec2> resized;
        extern EventPort<int> closed;
        extern float fps;
    }

    namespace window {
        extern std::string title;
        extern glm::vec2 size;
        extern glm::vec2 position;
        void close();
    }

    namespace input {
        glm::vec2 mouse_xy();

        extern EventPort<glm::vec2> mouse_moved; // delta
        extern EventPort<int> key_pressed;
        extern EventPort<int> key_released;
        extern EventPort<int> mouse_clicked;
        extern EventPort<int> mouse_released; 
    }
}