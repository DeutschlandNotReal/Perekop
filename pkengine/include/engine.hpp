#pragma once
#include "events.hpp"
#include "geometry.hpp"
#include <string>

namespace pk::engine {
    namespace window {
        extern EventPort<double>& step;
        extern EventPort<glm::vec2>& resized;
        extern void close();
        extern void resize(glm::vec2 new_size);
        extern void rename(std::string new_title);
        extern glm::vec2 get_size();
    }

    namespace scene {
        extern Mesh* new_mesh();
        extern const Camera camera;
    }

    namespace input {
        extern glm::vec2 mouse_xy();

        extern EventPort<glm::vec2>& mouse_moved; // parameter is delta
        extern EventPort<int>& input_began;
        extern EventPort<int>& input_ended;
    }

    namespace game {
        extern void launch();
        extern void close();
    }
}

#undef exc