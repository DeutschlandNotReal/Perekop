#pragma once
#include <PK/pose.hpp>
#include <PK/event.hpp>

namespace Perekop::Mouse {
    enum Button { left = 0, right = 1, middle = 2 };
    inline vec2 pos{0,0};
    void point_to(vec3);

    extern void lock(); 
    extern void unlock();
    extern bool is_locked();

    inline pk::pose pose;

    inline pk::Event<vec2> on_move;
    inline pk::Event<Button> on_down, on_up;
    inline pk::Event<int> on_scroll;
    extern bool held(Button button);
}

namespace Perekop::Input {
    inline pk::Event<int> on_down, on_up;
    extern bool held(int key);
}