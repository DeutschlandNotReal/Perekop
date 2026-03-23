#pragma once
#include "events.hpp"
#include "mesh.hpp"

namespace pk { namespace engine {
    extern MeshRenderer mesh_renderer;

    namespace frame {
        extern EventPort<float> stepped;
        extern EventPort<glm::vec2> resized;
        extern EventPort<int> closed; // pretty sure closing it just gonna stop everything anyways
    }
}}