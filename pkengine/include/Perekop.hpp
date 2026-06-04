#pragma once
#include <pk/Geometry.hpp>
#include <pk/Gui.hpp>
#include <pk/Body.hpp>
#include <pkutil/Event.hpp>

namespace Perekop {
    extern void on_launch();
    extern void on_exit();
    extern void on_step(double dt);
    extern void exit();

    namespace Mouse {
        enum Button { left = 0, right = 1, middle = 2 };
        inline glm::vec2 pos{0,0};
        void point_to(glm::vec2 point);

        extern void lock(); 
        extern void unlock();
        extern bool is_locked();

        extern glm::vec3 fvec();
        extern glm::mat3 matrix();
        inline pk::Event<glm::vec2> on_move;
        inline pk::Event<Button> on_down, on_up;
        inline pk::Event<int> on_scroll;
        extern bool held(Button button);
    }

    namespace Input {
        inline pk::Event<int> on_down, on_up;
        extern bool held(int key);
    }

    namespace Window {
        inline pk::Event<glm::vec2> resized;
        extern glm::vec2 get_size();
        extern void set_size(glm::vec2 size);
        extern void set_title(cstring title);

        extern void maximize();
        extern void minimize();
    }

    namespace Gui {
        inline pk::GUIObject* top{nullptr};
        inline pk::Array<pk::GUIObject> items;
    }

    namespace World {   
        inline pk::Set<pk::Model> models;
        inline pk::Set<pk::Mesh> meshes;
        inline pk::Set<pk::Body> bodies;
        inline glm::vec3 bgcol{0.2,0.2,0.2};

        inline double fps{50};
        inline pk::Camera camera;
    }
}