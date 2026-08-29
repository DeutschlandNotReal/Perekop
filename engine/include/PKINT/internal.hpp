#pragma once

#ifdef PK_INTERNAL
namespace Perekop { void render(bool); }
#include <PKSTL/vector.hpp>
#include <PK/step.hpp>
#include <PK/userinput.hpp>
#include <PK/window.hpp>

template <typename... T> using consumer = std::function<void(T...)>;
template <typename... T> using listeners = pk::vector<consumer<T...>>;

template <typename... T, typename... A>
void InvokeListeners(const listeners<T...>& l, A&&... args) noexcept {
    for (const auto &f: l) f(std::forward<A>(args)...);
}

#define ImplementBinder(name, source, ...) void name(consumer<__VA_ARGS__>&& f) noexcept { source.push(std::forward<decltype(f)>(f)); }

namespace Perekop {
    void RenderBegin() noexcept;
    void WindowBegin() noexcept;

    void RenderStep() noexcept;
    void PhysicsStep(double dt) noexcept;
}
#endif