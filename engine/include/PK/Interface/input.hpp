#pragma once
#include <PK/Util/event.hpp>
#include <PK/Math/transform.hpp>

namespace Perekop::Mouse {
    enum Button { left = 0, right = 1, middle = 2 };
    inline pk::vec2 pos{0,0};
    void point_to(pk::vec4);

    extern void lock(); 
    extern void unlock();
    extern bool is_locked();

    extern pk::vec4 fvec();
    extern mat3 matrix();
    inline pk::Event<pk::vec2> on_move;
    inline pk::Event<Button> on_down, on_up;
    inline pk::Event<i32> on_scroll;
    extern bool held(Button button);
}

namespace Perekop::Input {
    inline pk::Event<i32> on_down, on_up;
    extern bool held(i32 key);
}