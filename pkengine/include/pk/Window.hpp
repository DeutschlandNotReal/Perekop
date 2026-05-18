#pragma once
#include <pkutil/Event.hpp>
#include <glm/vec2.hpp>

struct GLFWwindow;

namespace pk {
    class Window;
    class Mouse {
            friend Window;
            GLFWwindow* _w;
            Mouse(GLFWwindow* window): _w(window) {}

            public:
                glm::vec2 pos() const noexcept;
                void pos(glm::vec2) const noexcept;

                void lock() const noexcept;
                void hide() const noexcept;
                void reset() const noexcept; // both unhides and unlocks

                pkutil::Event<>
                    left_down,
                    left_up,
                    right_down,
                    right_up,
                    middle_down,
                    middle_up;

                pkutil::Event<int, int> on_move;

                bool left_held() const noexcept;
                bool right_held() const noexcept;
                bool middle_held() const noexcept;
    };

    class Keyboard {
            friend Window;
            GLFWwindow* _w;
            Keyboard(GLFWwindow* window): _w(window) {}
            public:
                pkutil::Event<int> 
                    key_down,
                    key_up;

                bool key_held(int key) const noexcept;
    };

    class Window {
        friend Keyboard;
        friend Mouse;
        GLFWwindow* _w;

        public:
            Window(): _w(nullptr) {}
            Window(const char* title, int width, int height);
            operator GLFWwindow*() const noexcept { return _w; }

            Mouse mouse{_w};
            Keyboard keyboard{_w};

            glm::vec2 position() const noexcept;
            void position(glm::vec2 p) const noexcept;

            glm::vec2 size() const noexcept;
            void size(glm::vec2 s) const noexcept;

            const char* title() const noexcept;
            void title(const char* t) const noexcept;

            pkutil::Event<int, int> 
                on_resize,
                on_move;

            pkutil::Event<> 
                on_minimize,
                on_maximize,
                on_close;

            void maximize() const noexcept;
            void minimize() const noexcept;
            void close() const noexcept;

            void attention() const noexcept;
            void focus() const noexcept;
            void set_context() const noexcept;
            void swap() const noexcept;
            void show() const noexcept;

            bool should_close() const noexcept;
            bool visible() const noexcept;
    };
}