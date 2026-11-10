#pragma once
#include <PK/Render/Camera.hpp>
#include <PK/world.hpp>

namespace PKG {
    class GameCam : public pk::Camera {
        float pitch{0}, yaw{0};

        public:
            pk::pose rel;
            enum class Type { direct, orbit, follow };
            Type type{Type::orbit};
            pk::id<pk::ModelHandle> follow;

            void init() noexcept;
            void step(float dt) noexcept;

            void target(pk::ModelHandle&) noexcept;
            void target() noexcept;
    };
} 