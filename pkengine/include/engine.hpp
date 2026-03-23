#pragma once
#include "events.hpp"
#include "mesh.hpp"
#include <string>

namespace pk::engine {
    extern MeshRenderer mesh_renderer;

    namespace frame {
        extern EventPort<float> stepped;
        extern EventPort<glm::vec2> resized;
        extern EventPort<int> closed; // pretty sure closing it just gonna stop everything anyways
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

        extern EventPort<glm::vec2> mouse_moved; // moved from last frame, not new position!!
        extern EventPort<int> key_pressed;
        extern EventPort<int> key_released;
        extern EventPort<int> mouse_clicked; // int for these 2 mean nothing cant have void events
        extern EventPort<int> mouse_released; 
    }
}