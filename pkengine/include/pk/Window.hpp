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
            Event<> e_ldown, e_rdown, e_mdown, e_lup, e_rup, e_mup;
            Event<int, int> moved;
            public:
                glm::vec2 pos() const noexcept;
                void pos(glm::vec2) const noexcept;

                void lock() const noexcept;
                void hide() const noexcept;
                void reset() const noexcept; // both unhides and unlocks

                EventPort<>
                left_down{e_ldown},
                left_up{e_lup},
                right_down{e_rdown},
                right_up{e_rup},
                middle_down{e_mdown},
                middle_up{e_mup};

                EventPort<int, int> onMove{moved};

                bool left_held() const noexcept;
                bool right_held() const noexcept;
                bool middle_held() const noexcept;
    };

    class Keyboard {
            friend Window;
            GLFWwindow* _w;
            Keyboard(GLFWwindow* window): _w(window) {}
            Event<int> e_down, e_up;
            public:
                EventPort<int> 
                    key_down{e_down},
                    key_up{e_up};

                bool key_held(int key) const noexcept;
    };

    class Window {
        friend Keyboard;
        friend Mouse;
        GLFWwindow* _w;

        Event<int, int> resized, moved;
        Event<> minimized, maximized, closed;
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

            EventPort<int, int> 
                on_resize{resized},
                on_move{moved};
            EventPort<> 
                on_minimize{minimized},
                on_maximize{maximized},
                on_close{closed};

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