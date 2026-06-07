#pragma once
#include <PKCore/String.hpp>
#include <PKLib/Geometry.hpp>
#include <PKLib/Gui.hpp>
#include <PKLib/Body.hpp>
#include <PKLib/Event.hpp>

namespace Perekop {
    extern void on_launch();
    extern void on_exit();
    extern void on_step(double dt);
    extern void exit();

    namespace Mouse {
        enum Button { left = 0, right = 1, middle = 2 };
        inline vec2 pos{0,0};
        void point_to(vec3);

        extern void lock(); 
        extern void unlock();
        extern bool is_locked();

        extern vec3 fvec();
        extern mat3 matrix();
        inline pk::Event<vec2> on_move;
        inline pk::Event<Button> on_down, on_up;
        inline pk::Event<int> on_scroll;
        extern bool held(Button button);
    }

    namespace Input {
        inline pk::Event<int> on_down, on_up;
        extern bool held(int key);
    }

    namespace Window {
        inline pk::Event<vec2> on_resize;

        extern vec2 get_size();
        extern void set_size(vec2 size);
        extern void set_title(pk::stringview title);

        extern void maximize();
        extern void minimize();
    }

    namespace Gui {
        inline pk::GUIObject* top{nullptr};
        inline pk::dynarray<pk::GUIObject> items;
    }

    namespace World {   
        inline pk::set<pk::Model> models;
        inline pk::set<pk::Mesh> meshes;
        inline pk::set<pk::Body> bodies;
        inline glm::vec3 bgcol{0.2,0.2,0.2};

        inline double fps{50};
        inline pk::Camera camera;
    }
}