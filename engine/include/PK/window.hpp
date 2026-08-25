#pragma once
#include <PK/event.hpp>
#include <PK/pose.hpp>
#include <string_view>

namespace Perekop::Window {
        inline pk::Event<vec2> on_resize;

        extern vec2 get_size() noexcept;
        extern void set_size(vec2 size) noexcept;
        extern void set_title(std::string_view title) noexcept;

        extern void maximize() noexcept;
        extern void minimize() noexcept;
}

#ifdef PK_ENGINE_SRC
struct GLFWwindow;
namespace Perekop::Window {
        inline GLFWwindow* glfw;
}
#endif