#pragma once
#include <PK/Render/camera.hpp>
#include <PK/world.hpp>

namespace PKG {
    class GameCam : public pk::camera {
        float pitch{0}, yaw{0};

        public:
            pk::pose rel;
            enum class Type { direct, orbit, follow };
            enum class FollowType { none, body, model };
            Type type{Type::orbit};
            FollowType ftype{FollowType::none};
            uint fid{0};

            void init() noexcept;
            void step(float dt) noexcept;

            void target(pk::entry<pk::model>) noexcept;
            void target(pk::entry<pk::body>) noexcept;
            void target() noexcept;
    };
} 