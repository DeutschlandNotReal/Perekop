#pragma once
#include <PK/Core/array.hpp>

namespace PKG {
    namespace onInit {
        void window();
        void render();
        void assets();
        void world();
        void camera();

        inline auto runorder = {
            *assets,
            *window,
            *render,
            *world,
            *camera
        };
    }

    namespace onStep {
        void camera(float);
        void world(float);

        inline auto runorder = {
            *camera,
            *world
        };
    }

    namespace onExit {

    }
}