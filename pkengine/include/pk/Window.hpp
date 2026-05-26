#pragma once
#include <pkutil/Event.hpp>
#include <glm/vec2.hpp>

class GLFWwindow;
namespace pk {
    class Window {
        GLFWwindow* ptr{nullptr};

        public:
            Window() = default;
            Window(const char* title, int width, int height);

            class Input {
                friend Window;
                GLFWwindow*& ptr;
                glm::vec2 lpos{0};
                public:
                    enum CursorMode { captured = 0x00034004, hidden = 0x00034002, normal = 0x00034001};
                    Input(GLFWwindow*& ptr): ptr(ptr) {}
                    glm::vec2 pos() const; void pos(glm::vec2);

                    Event<int>
                        on_down_m, on_up_m,
                        on_down_k, on_up_k;

                    Event<double> on_scroll_m;
                    Event<glm::vec2> on_move_m;
                    bool is_held_m(int button) const;
                    bool is_held_k(int key) const;
                    void mouse_mode(CursorMode mode) const;
            };

            Input input{ptr};

            glm::vec2 pos() const;
            void pos(glm::vec2) const;

            glm::vec2 size() const;
            void size(glm::vec2) const;

            const char* title() const;
            void title(const char*) const;

            Event<int, int> 
                on_resize,
                on_move;

            Event<> 
                on_minimize,
                on_maximize,
                on_close;

            void maximize() const;
            void minimize() const;
            void close() const;

            void attention() const;
            void focus() const;
            void set_context() const;
            void swap() const;
            void show() const;

            bool should_close() const;
            bool visible() const;
    };
}