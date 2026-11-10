#pragma once
#include <PK/Core/array.hpp>

namespace PKG {
    namespace onInit {
        void window();
        void render();
        void assets();
        void world();
        void Camera();

        inline auto runorder = {
            *assets,
            *window,
            *render,
            *world,
            *Camera
        };
    }

    namespace onStep {
        void Camera(float);
        void world(float);

        inline auto runorder = {
            *Camera,
            *world
        };
    }

    namespace onExit {

    }
}