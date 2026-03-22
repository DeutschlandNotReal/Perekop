#pragma once
#include "events.hpp"
#include "mesh.hpp"

namespace perekop {
    extern PKRenderer mesh_renderer;

    namespace events {
        extern PKEventPort<float> tick_stepped;
        extern PKEventPort<float> frame_stepped;
    }
}