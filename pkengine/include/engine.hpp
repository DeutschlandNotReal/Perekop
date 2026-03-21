#pragma once
#include "events.hpp"
#include "mesh.hpp"

namespace PKEngine {
    extern PKRenderer Renderer;

    namespace Events {
        extern PKEventPort<float> NewTick;
        extern PKEventPort<float> NewFrame;
    }
}