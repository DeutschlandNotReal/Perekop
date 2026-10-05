#pragma once
#include <functional>

namespace Perekop::Input {
    extern void BindToPress(std::function<void()>&&) noexcept;
    extern void BindToRelease(std::function<void()>&&) noexcept;

    extern bool Held(int) noexcept;
}