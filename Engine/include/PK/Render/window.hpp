#pragma once
#include <PK/Math/pose.hpp>
#include <PK/Util/events.hpp>
#include <PK/Render/Camera.hpp>

#include <functional>
#include <string_view>
#include <filesystem>

struct GLFWwindow;

namespace pk {
    enum class MouseKey { left = 0, right, middle };

    class Window {
        GLFWwindow* handle{nullptr};

        vec2 lmpos  {0.f}; // last mouse position
        bool nomove {false};

        public:
            Event<vec2>     onMouseMove;
            Event<float>    onMouseScroll;
            Event<MouseKey> onMouseDown;
            Event<MouseKey> onMouseUp;
            Event<int>      onKeyDown;
            Event<int>      onKeyUp;
            Event<>         onRefresh;
            Event<vec2>     onResize;

            Window() = default;
            Window(const Window&) = delete;
            Window& operator=(const Window&) = delete;
            ~Window() noexcept;

            operator GLFWwindow*() const noexcept { return handle; }

            Camera* camera{nullptr};
    
            void mousePos(vec2) noexcept;
            vec2 mousePos()     const noexcept;
            pose mousePose()    const noexcept;

            bool held(int key) const noexcept;
            bool held(MouseKey key) const noexcept;

            void setMouseLocked(bool) noexcept;
            bool isMouseLocked() const noexcept;

            bool create(int width, int height, const char* title) noexcept;

            void setTitle(std::string_view) noexcept;
            bool setIcon(const std::filesystem::path&) noexcept;
            void setSize(vec2) noexcept;
            void maximize() noexcept;
            void minimize() noexcept;
            vec2 size() const noexcept;
            void exit() noexcept;
            void show() noexcept;
            bool closing() const noexcept;
            void makeContextCurrent() noexcept;

            static Window main;
    };

    inline constinit Window Window::main;
}