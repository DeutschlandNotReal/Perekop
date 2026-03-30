#pragma once
#include <Perekop/Event.hpp>
#include <Perekop/Geometry.hpp>
#include <string>

#define Port extern pk::EventPort
namespace Perekop {
    namespace Window {
        extern float FPS;
        Port<double>& step;
        Port<int, int>& resized;
        extern glm::vec2 get_size();
    }

    namespace Scene {
        extern pk::Mesh* new_mesh();
        extern pk::Camera camera;
    }

    namespace Input {
        extern glm::vec2 mouse_xy();

        Port<glm::vec2>& mouse_moved; // parameter is delta
        Port<int>& input_began;
        Port<int>& input_ended;
    }

    namespace Game {
        extern void launch();
        extern void close();
    }
}
#undef Port