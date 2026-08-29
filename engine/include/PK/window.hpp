#pragma once
#include <PK/pose.hpp>
#include <string_view>
#include <functional>
#include <filesystem>

namespace Perekop::Window {
        extern void BindToResize(std::function<void(vec2)>&&) noexcept;
        extern void SetTitle(std::string_view) noexcept;
        extern void SetIcon(const std::filesystem::path&) noexcept;
        extern void SetSize(vec2) noexcept;
        extern void Maximize() noexcept;
        extern void Minimize() noexcept;

        extern vec2 Size() noexcept;
}

#ifdef PK_INTERNAL
struct GLFWwindow;
namespace Perekop { inline GLFWwindow* glfw_window; }
#endif