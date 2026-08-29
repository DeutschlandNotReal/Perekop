#pragma once
#include <PK/pose.hpp>
#include <functional>

namespace Perekop::Mouse {
    enum Button { left = 0, right = 1, middle = 2 };
    extern void SetPosition() noexcept;
    extern void Lock() noexcept;
    extern void Unlock() noexcept;
    extern bool Locked() noexcept;
    
    extern vec2 Position() noexcept;
    extern void BindToMove(std::function<void(vec2)>&&) noexcept;
    extern void BindToScroll(std::function<void(float)>&&) noexcept;
    extern void BindToPress(std::function<void(Button)>&&) noexcept;
    extern void BindToRelease(std::function<void(Button)>&&) noexcept;

    extern bool Held(Button) noexcept;
}

namespace Perekop::Input {
    extern void BindToPress(std::function<void()>&&) noexcept;
    extern void BindToRelease(std::function<void()>&&) noexcept;

    extern bool Held(int) noexcept;
}