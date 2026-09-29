#pragma once
#include <PK/Render/camera.hpp>

namespace PKG {
    void InitCamera(pk::Camera& camera) noexcept;
    void StepCamera(pk::Camera& camera, float dt) noexcept;
}