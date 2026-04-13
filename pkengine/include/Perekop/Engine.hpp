#pragma once
#include <Perekop/Event.hpp>
#include <Perekop/Model.hpp>
#include <Perekop/Mesh.hpp>

#include <string>

#define Port extern pk::EventPort

namespace Perekop {
    namespace Window {
        extern float FPS;
        Port<int, int>& resized;
        Port<double>& step;
        
        [[nodiscard]] extern glm::vec2 size();
        extern void set_size(glm::vec2 d);

        [[nodiscard]] extern std::string get_title();
        extern void set_title(const std::string& title);

        extern void maximize();
        extern void minimize();
        extern void close();
    }

    namespace Scene {
        extern pk::Mesh* new_mesh();
        extern pk::Mesh* new_mesh(pk::MeshMaterial mat);
        extern pk::Camera camera;
    }

    namespace Mouse {
        [[nodiscard]] glm::vec2 pos();
        extern void set_pos(glm::vec2 pos);

        Port<>& left_pressed;
        Port<>& left_released;
        Port<>& right_pressed;
        Port<>& right_released;
        Port<double, double>& move;
        extern bool right_down();
        extern bool left_down();
    }

    namespace Input {
        Port<int>& key_pressed;
        Port<int>& key_released;
        [[nodiscard]] std::string get_clipboard();
        extern void set_clipboard(const std::string& content);
        extern bool key_down(int key);
    }

    namespace Game {
        extern void launch();
        extern void close();
    }
}
#undef Port