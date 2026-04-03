#pragma once
#include <Perekop/Event.hpp>
#include <Perekop/Model.hpp>
#include <Perekop/Mesh.hpp>

#define Port extern pk::EventPort

namespace Perekop {
    namespace Window {
        extern float FPS;
        Port<int, int>& resized;
        Port<double>& step;
        
        [[nodiscard]] extern glm::vec2 get_size();
        extern void get_size(int& x, int& y);
        extern void set_size(glm::vec2 d);
        extern void set_size(int x, int y);

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

        Port<>& rmb_down;
        Port<>& rmb_up;
        Port<>& lmb_down;
        Port<>& lmb_up;
        Port<double, double>& move;
        extern bool is_rmb_down();
        extern bool is_lmb_down();
    }

    namespace Input {
        Port<int>& key_down;
        Port<int>& key_up;
        [[nodiscard]] std::string get_clipboard();
        extern void set_clipboard(const std::string& content);
        extern bool is_key_down(int key);
    }

    namespace Game {
        extern void launch();
        extern void close();
    }
}
#undef Port