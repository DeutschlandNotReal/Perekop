#pragma once

#ifdef PK_INTERNAL
#include <PK/tick.hpp>

namespace Perekop {
    void renderBegin() noexcept;

    void renderStep() noexcept;
    void physicsStep(float dt) noexcept;
}
#endif