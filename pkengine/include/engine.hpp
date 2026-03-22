#pragma once
#include "events.hpp"
#include "mesh.hpp"

namespace pk { namespace engine {
    extern PKRenderer mesh_renderer;

    namespace events {
        extern event_port<float> tick_stepped;
        extern event_port<float> frame_stepped;
    }
}}