#pragma once
#include "events.hpp"
#include "geometry.hpp"
#include <string>
#include <vector>

namespace pk::engine {
    namespace window {
        extern const EventPort<double>& step;
        extern const EventPort<glm::vec2>& resized;
        extern const EventPort<>& began;
        extern const EventPort<>& ended;
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

        extern const EventPort<glm::vec2>& mouse_moved; // parameter is delta
        extern const EventPort<int>& input_began;
        extern const EventPort<int>& input_ended;
    }
}

#undef exc