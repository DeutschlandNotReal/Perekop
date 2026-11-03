#pragma once
#include <PK/Render/camera.hpp>
#include <PK/Scene/world.hpp>

namespace PKG {
    class GameCamera : public pk::Camera {
        float pitch{0}, yaw{0};

        public:
            pk::pose rel;
            enum class Type { direct, orbit, follow };
            enum class FollowType { none, body, model };
            Type type{Type::orbit};
            FollowType ftype{FollowType::none};
            uint fid{0};

            void Init() noexcept;
            void Step(float dt) noexcept;

            void Target(const pk::entry<pk::Model>) noexcept;
            void Target(const pk::entry<pk::Body>) noexcept;
            void Target() noexcept;
    };
}