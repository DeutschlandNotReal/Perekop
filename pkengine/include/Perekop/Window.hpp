#pragma once
#include <Perekop/Event.hpp>
#include <glm/vec2.hpp>

struct GLFWwindow;

namespace pk {
    class Window;

    class Mouse {
            friend Window;
            Window* win;

            Mouse(Window* w): win(w) {}
            Event<> l_down, l_up, m_down, m_up, r_down, r_up;
            Event<int, int> moved;

            bool is_down(int k) const noexcept;

            public:
                enum MouseKey {
                    left = 0,
                    right,
                    middle
                };
                
                glm::vec2 pos() const;
                void set_pos(glm::vec2) const;

                void lock() const;
                void hide() const;
                void reset() const; // both unhides and unlocks

                template <MouseKey key> EventPort<> on_press() {
                    if constexpr(key==0) return l_down.port;
                    if constexpr(key==1) return r_down.port;
                    if constexpr(key==2) return m_down.port;
                }

                template <MouseKey key> EventPort<> on_release() {
                    if constexpr(key==0) return l_up.port;
                    if constexpr(key==1) return r_up.port;
                    if constexpr(key==2) return m_up.port;
                }

                template <MouseKey key> bool is_held() {
                    return is_down(key);
                }

                EventPort<int, int> on_move{moved.port};
    };

    class Keyboard {
            friend Window;
            Window* win;

            Keyboard(Window* w): win(w) {}
            Event<int> key_down, key_up;
            public:
                EventPort<int> 
                    key_pressed{key_down.port},
                    key_released{key_up.port};

                bool is_held(int key) const;
    };

    class Window {
        friend Keyboard;
        friend Mouse;
        GLFWwindow* win{nullptr};

        Event<int, int> resized, moved;
        Event<> minimized, maximized, closed;
        public:
            Window() = default;
            Window(const char* title, int width, int height);
            operator GLFWwindow*() const noexcept { return win; }

            Mouse mouse{this};
            Keyboard keyboard{this};

            glm::vec2 pos() const;
            void set_pos(glm::vec2 p) const;
            glm::vec2 size() const;
            void set_size(glm::vec2 s) const;
            const char* title() const;
            void set_title(const char* t) const;

            EventPort<int, int> 
                on_resize{resized.port},
                on_move{moved.port};
            EventPort<> 
                on_minimize{minimized.port},
                on_maximize{maximized.port},
                on_close{closed.port};

            void maximize() const;
            void minimize() const;
            void close() const;

            void attention() const;
            void focus() const;
            void make_context() const;
            void swap_buffers() const;
            void show() const;

            bool should_close() const;
    };
}