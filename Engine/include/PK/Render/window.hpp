#pragma once
#include <PK/Math/pose.hpp>
#include <PK/Util/events.hpp>
#include <PK/Render/camera.hpp>

#include <functional>
#include <string_view>
#include <filesystem>

struct GLFWwindow;

namespace pk {
    enum class mouseButton { left = 0, right, middle };

    class window {
        GLFWwindow* handle{nullptr};
        void installCallbacks() noexcept;
        GLFWwindow* native() const noexcept { return handle; }

        listeners<> refreshListeners;
        listeners<vec2> resizeListeners;

        public:
            class input {
                friend class window;
                window* owner;
                vec2 lastPosition{0.f};
                bool skipMove{false};

                listeners<vec2> mouseMoveListeners;
                listeners<float> mouseScrollListeners;
                listeners<mouseButton> mousePressListeners;
                listeners<mouseButton> mouseReleaseListeners;
                listeners<int> keyPressListeners;
                listeners<int> keyReleaseListeners;

                explicit input(window& owner) noexcept: owner(&owner) {}

                public:
                    void setPosition(vec2) noexcept;
                    void lockMouse() noexcept;
                    void unlockMouse() noexcept;
                    bool mouseLocked() const noexcept;
                    vec2 mousePosition() const noexcept;
                    pose mousePose() const noexcept;
                    bool mouseHeld(mouseButton) const noexcept;
                    bool keyHeld(int) const noexcept;

                    void bindMove(consumer<vec2>) noexcept;
                    void bindScroll(consumer<float>) noexcept;
                    void bindMPress(consumer<mouseButton>) noexcept;
                    void bindMRelease(consumer<mouseButton>) noexcept;
                    void bindKPress(consumer<int>) noexcept;
                    void bindKRelease(consumer<int>) noexcept;
            };

            input input;
            camera* mainCamera{nullptr};

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

            void connectRefresh(consumer<>) noexcept;
            void connectResize(consumer<vec2>) noexcept;

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