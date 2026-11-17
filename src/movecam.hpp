#pragma once
#include <PK/Render/Camera.hpp>
#include <PK/world.hpp>

namespace PKG {
    class MovableCamera : public pk::Camera {
        float pitch{0}, yaw{0};

        public:
            pk::pose rel;
            enum class Type { direct, orbit, follow };
            Type type{Type::orbit};
            pk::ModelID follow;

            void connect() noexcept;
            void step(float dt) noexcept;

            void target(pk::ModelHandle&) noexcept;
            void target() noexcept;
    };
} 