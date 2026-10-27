#pragma once
#include <PK/Physics/body.hpp>

namespace pk::Physics {
    struct CollisionGroup {
        vector<unsigned> modelIds;
    };

    void Collide(Body& A, Body& B, float dt) noexcept;
}
