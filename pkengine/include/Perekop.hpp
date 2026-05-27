#pragma once

#include <pk/Mesh.hpp>
#include <pkutil/Event.hpp>

namespace pk {
    struct GUIElement { float Z; glm::vec2 pos{0}, size{0}; glm::vec3 col{0.5, 1, 1}; };
}

namespace Perekop {
    extern void on_launch();
    extern void on_exit();
    extern void on_step(double dt);
    extern void exit();

    namespace Mouse {
        enum State { captured = 0x00034004, hidden = 0x00034002, normal = 0x00034001 };

        extern glm::vec2 position(); extern void position(glm::vec2 position);
        extern glm::vec3 fvec();
        inline pk::Event<glm::vec2> on_move;
        inline pk::Event<int> on_down, on_up, on_scroll;
        extern bool held(int button);
        extern void set(State mouse_state);
    }

    namespace Input {
        inline pk::Event<int> on_down, on_up;
        bool held(int key);
    }

    namespace Window {
        extern pk::Array<pk::GUIElement> gui;
        extern glm::vec2 size(); extern void size(glm::vec2 size);
        extern glm::vec2 position(); extern void position(glm::vec2 position);
        extern const char* title(); extern void title(const char* title);

        extern void maximize();
        extern void minimize();
    }

    namespace World {
        inline pk::SparseSet<pk::Model> models;
        inline pk::SparseSet<pk::Mesh> meshes;
        inline double fps{60};
        inline pk::Camera camera;
    }
}