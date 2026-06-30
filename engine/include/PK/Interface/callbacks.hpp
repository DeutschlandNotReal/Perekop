#pragma once
#include <PK/Core/number.hpp>

namespace Perekop {
    // defined by game
    extern void on_launch();
    extern void on_exit();
    extern void on_step(f64 dt);

    // defined by engine
    extern void exit();
}