#pragma once

namespace Perekop {
    inline double fps{60};

    // defined by game
    extern void OnLaunch();
    extern void OnExit();
    extern void OnStep(double dt);
    extern void OnRender();

    // defined by engine
    extern void ExitGame();
}