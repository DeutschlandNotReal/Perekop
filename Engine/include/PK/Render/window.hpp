#pragma once
#include <PK/Math/pose.hpp>
#include <PK/Util/events.hpp>
#include <PK/Render/Camera.hpp>

#include <functional>
#include <string_view>
#include <filesystem>

struct GLFWwindow;

namespace pk {
    enum class mouseButton { left = 0, right, middle };

    class window {
        GLFWwindow* handle{nullptr};
        void installCallbacks() noexcept;

        public:
            class input {
                friend class window;
                window* owner;
                vec2 lastPosition{0.f};
                bool skipMove{false};

                explicit input(window& owner) noexcept: owner(&owner) {}

                public:
                    Event<window, vec2>         onMove;
                    Event<window, float>        onScroll;
                    Event<window, mouseButton>  onMouseDown;
                    Event<window, mouseButton>  onMouseUp;
                    Event<window, int>          onKeyDown;
                    Event<window, int>          onKeyUp;

                    void setPosition(vec2) noexcept;
                    void lockMouse() noexcept;
                    void unlockMouse() noexcept;
                    bool mouseLocked() const noexcept;
                    vec2 mousePosition() const noexcept;
                    pose mousePose() const noexcept;
                    bool mouseHeld(mouseButton) const noexcept;
                    bool keyHeld(int) const noexcept;
            };

            input input;
            Camera* mainCamera{nullptr};

            window() noexcept;
            window(const window&) = delete;
            window& operator=(const window&) = delete;
            ~window() noexcept;

            bool valid() const noexcept { return handle != nullptr; }
            bool create(int width, int height, const char* title) noexcept;

            static bool init() noexcept;
            static void terminate() noexcept;
            static void setContextVersion(int major, int minor) noexcept;
            using ProcAddress = void (*)(void);
            static ProcAddress getProcAddress(const char* name) noexcept;
            static void pollEvents() noexcept;

            Event<window>        onRefresh;
            Event<window,vec2>   onResize;
    
            void setTitle(std::string_view) noexcept;
            bool setIcon(const std::filesystem::path&) noexcept;
            void setSize(vec2) noexcept;
            void maximize() noexcept;
            void minimize() noexcept;
            vec2 size() const noexcept;
            void exit() noexcept;
            void show() noexcept;
            bool shouldClose() const noexcept;
            void makeContextCurrent() noexcept;
            void swapInterval(int interval) noexcept;
            void swapBuffers() noexcept;
    };

    inline window mainWindow{};
}