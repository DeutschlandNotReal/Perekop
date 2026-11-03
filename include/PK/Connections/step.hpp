#pragma once

namespace Perekop {
    inline double fps{240};

    // defined by game
    extern void OnLaunch();
    extern void OnExit();
    extern void OnStep(float dt);

    extern void PreRender();
    extern void PostRender();

    // defined by engine
    extern void ExitGame();
}