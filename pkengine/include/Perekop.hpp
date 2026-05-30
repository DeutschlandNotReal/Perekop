#pragma once

#include <pk/Geometry.hpp>
#include <pk/GUI.hpp>
#include <pkutil/Event.hpp>

namespace Perekop {
    extern void on_launch();
    extern void on_exit();
    extern void on_step(double dt);
    extern void exit();

    namespace Mouse {
        enum State { captured = 0x00034004, hidden = 0x00034002, normal = 0x00034001 };
        inline glm::vec2 pos{0,0};
        
        extern glm::vec3 fvec();
        extern glm::mat3 matrix();
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
        inline glm::vec2 size{0,0}, pos{0, 0};
        inline const char* title;
        
        extern void maximize();
        extern void minimize();
    }

    namespace GUI {
        inline pk::GUIObject* top{nullptr};
        inline pk::Array<pk::GUIObject> gui;
        inline pk::Event<pk::GUIObject*> on_enter, on_exit;
        inline pk::Event<pk::GUIObject*, int> on_down, on_up;
    }

    namespace World {   
        inline pk::SparseSet<pk::Model> models;
        inline pk::SparseSet<pk::Mesh> meshes;
        inline glm::vec3 background_colour{0.2,0.2,0.2};

        inline double fps{60};
        inline pk::Camera camera;
    }
}