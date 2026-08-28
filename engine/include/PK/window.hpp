#pragma once
#include <PK/event.hpp>
#include <PK/pose.hpp>
#include <string_view>
#include <filesystem>

namespace Perekop::Window {
        inline pk::Event<vec2> on_resize;

        extern vec2 size() noexcept;
        extern void size(vec2) noexcept;

        extern void title(std::string_view) noexcept;

        extern void maximize() noexcept;
        extern void minimize() noexcept;

        extern void icon(const std::filesystem::path&) noexcept;
        extern void swap_buffers() noexcept;
        extern void clear(vec3 colour) noexcept;
}

#ifdef PK_ENGINE_SRC
struct GLFWwindow;
namespace Perekop::Window {
        inline GLFWwindow* glfw;
}
#endif