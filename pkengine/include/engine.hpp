#pragma once
#include "events.hpp"
#include "mesh.hpp"

namespace pk { namespace engine {
    extern MeshRenderer mesh_renderer;

    namespace events {
        extern EventPort<float> tick_stepped;
        extern EventPort<float> frame_stepped;
    }
}}