#include <pk/Window.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace pk;

Window& get_window(GLFWwindow* ptr) {
    return *static_cast<Window*>(glfwGetWindowUserPointer(ptr));
}

glm::vec2 Mouse::pos() const noexcept {
    double x, y;
    glfwGetCursorPos(_w, &x, &y);
    return {x, y};
}

void Mouse::pos(glm::vec2 pos) const noexcept {
    glfwSetCursorPos(_w, pos.x, pos.y);
}

void Mouse::lock() const noexcept {
    glfwSetInputMode(_w, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
}

void Mouse::hide() const noexcept {
    glfwSetInputMode(_w, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Mouse::reset() const noexcept {
    glfwSetInputMode(_w, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

glm::vec2 Window::position() const noexcept {
    int x, y;
    glfwGetWindowPos(_w, &x, &y);
    return {x, y};
}

void Window::position(glm::vec2 p) const noexcept {
    glfwSetWindowPos(_w, p.x, p.y);
}

glm::vec2 Window::size() const noexcept {
    int x, y;
    glfwGetWindowSize(_w, &x, &y);
    return {x, y};
}

void Window::size(glm::vec2 s) const noexcept {
    glfwSetWindowSize(_w, s.x, s.y);
}

const char* Window::title() const noexcept { return glfwGetWindowTitle(_w); }
void Window::title(const char* t) const noexcept { glfwSetWindowTitle(_w, t); }

void Window::maximize() const noexcept { glfwMaximizeWindow(_w); }
void Window::minimize() const noexcept { glfwIconifyWindow(_w); }
void Window::close() const noexcept{ glfwDestroyWindow(_w); }
void Window::attention() const noexcept { glfwRequestWindowAttention(_w); }
void Window::focus() const noexcept { glfwFocusWindow(_w); }
void Window::set_context() const noexcept { glfwMakeContextCurrent(_w); }
void Window::show() const noexcept { glfwShowWindow(_w); }
void Window::swap() const noexcept { glfwSwapBuffers(_w); }
bool Window::should_close() const noexcept { return glfwWindowShouldClose(_w); }
bool Window::visible() const noexcept { return glfwGetWindowAttrib(_w, GLFW_VISIBLE); }

bool Keyboard::key_held(int key) const noexcept { return glfwGetKey(_w, key) == GLFW_PRESS; }
   
Window::Window(const char* title, int w, int h):
    _w(glfwCreateWindow(w, h, title, NULL, NULL))
{
    glfwSetWindowUserPointer(_w, this);

    // mouse move & press
    glfwSetCursorPosCallback(_w, [](GLFWwindow* glfw_win, double x, double y){
        get_window(glfw_win).mouse.on_move.fire(x, y);
    });

    glfwSetMouseButtonCallback(_w, [](GLFWwindow* glfw_win, int button, int action, int){
        Mouse& mouse = get_window(glfw_win).mouse;

        // button: [0: left, 1: right, 2: middle]
        // action: [0: released, 1: pressed]

        switch (button | (action << 4)) {
            case (0x10): return mouse.left_down.fire();
            case (0x11): return mouse.right_down.fire();
            case (0x13): return mouse.middle_down.fire();
            case (0x00): return mouse.left_up.fire();
            case (0x01): return mouse.right_up.fire();
            case (0x03): return mouse.middle_up.fire();
        }
    });

    // keyboard input
    glfwSetKeyCallback(_w, [](GLFWwindow* glfw_win, int key, int, int action, int){
        Keyboard& keyboard = get_window(glfw_win).keyboard;
        
        switch (action) {
            case GLFW_PRESS: return keyboard.key_down.fire(key);
            case GLFW_RELEASE: return keyboard.key_up.fire(key);
            case GLFW_REPEAT: break; // IGNORED!
        }
    });

    // window
    glfwSetWindowSizeCallback(_w, [](GLFWwindow* glfw_win, int x, int y){
        get_window(glfw_win).on_resize.fire(x, y);
    });

    glfwSetWindowPosCallback(_w, [](GLFWwindow* glfw_win, int x, int y){
        get_window(glfw_win).on_move.fire(x, y);
    });

    glfwSetWindowIconifyCallback(_w, [](GLFWwindow* glfw_win, int){
        get_window(glfw_win).on_minimize.fire();
    });

    glfwSetWindowMaximizeCallback(_w, [](GLFWwindow* glfw_win, int){
        get_window(glfw_win).on_maximize.fire();
    });

    glfwSetWindowCloseCallback(_w, [](GLFWwindow* glfw_win){
        get_window(glfw_win).on_close.fire();
    });
}