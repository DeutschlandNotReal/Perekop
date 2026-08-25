#pragma once

namespace Perekop {
    // defined by game
    extern void on_launch();
    extern void on_exit();
    extern void on_step(double dt);

    // defined by engine
    extern void exit();
}