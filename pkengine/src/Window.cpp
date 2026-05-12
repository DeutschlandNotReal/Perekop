#include <Perekop/Window.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace pk;

bool Mouse::is_down(int key) const noexcept {
    return glfwGetMouseButton(*win, key) == GLFW_PRESS;
}
 
glm::vec2 Mouse::pos() const {
    double x, y;
    glfwGetCursorPos(*win, &x, &y);
    return {x, y};
}

void Mouse::set_pos(glm::vec2 pos) const {
    glfwSetCursorPos(*win, pos.x, pos.y);
}

void Mouse::lock() const {
    glfwSetInputMode(*win, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
}

void Mouse::hide() const {
    glfwSetInputMode(*win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Mouse::reset() const {
    glfwSetInputMode(*win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

glm::vec2 Window::pos() const {
    int x, y;
    glfwGetWindowPos(win, &x, &y);
    return {x, y};
}

void Window::set_pos(glm::vec2 p) const {
    glfwSetWindowPos(win, p.x, p.y);
}

glm::vec2 Window::size() const {
    int x, y;
    glfwGetWindowSize(win, &x, &y);
    return {x, y};
}

void Window::set_size(glm::vec2 s) const {
    glfwSetWindowSize(win, s.x, s.y);
}

const char* Window::title() const {
    return glfwGetWindowTitle(win);
}

void Window::set_title(const char* t) const {
    glfwSetWindowTitle(win, t);
}

void Window::maximize() const { glfwMaximizeWindow(win); }
void Window::minimize() const { glfwIconifyWindow(win); }
void Window::close() const { glfwDestroyWindow(win); }
void Window::attention() const { glfwRequestWindowAttention(win); }
void Window::focus() const { glfwFocusWindow(win); }
void Window::make_context() const { glfwMakeContextCurrent(win); }
void Window::show() const { glfwShowWindow(win); }
void Window::swap_buffers() const { glfwSwapBuffers(win); }
bool Window::should_close() const { return glfwWindowShouldClose(win); }
bool Window::visible() const { return glfwGetWindowAttrib(win, GLFW_VISIBLE); }

bool Keyboard::is_held(int key) const { return glfwGetKey(*win, key) == GLFW_PRESS; }
   
Window::Window(const char* title, int w, int h): mouse(this), keyboard{this} {
    win = glfwCreateWindow(w, h, title, NULL, NULL);
    glfwSetWindowUserPointer(win, this);

    // mouse move & press
    glfwSetCursorPosCallback(win, [](GLFWwindow* glfw_win, double x, double y){
        static_cast<Window*>(glfwGetWindowUserPointer(glfw_win))->mouse.moved.invoke(x, y);
    });

    glfwSetMouseButtonCallback(win, [](GLFWwindow* glfw_win, int button, int action, int){
        Mouse& mouse = static_cast<Window*>(glfwGetWindowUserPointer(glfw_win))->mouse;

        // button: [0: left, 1: right, 2: middle]
        // action: [0: released, 1: pressed]

        switch (button | (action << 4)) {
            case (0x10): mouse.l_down.invoke(); break;
            case (0x11): mouse.r_down.invoke(); break;
            case (0x13): mouse.m_down.invoke(); break;

            case (0x00): mouse.l_up.invoke(); break;
            case (0x01): mouse.r_up.invoke(); break;
            case (0x03): mouse.m_up.invoke(); break;
        }
    });

    // keyboard input
    glfwSetKeyCallback(win, [](GLFWwindow* glfw_win, int key, int, int action, int){
        Keyboard& keyboard = static_cast<Window*>(glfwGetWindowUserPointer(glfw_win))->keyboard;

        if (action == GLFW_PRESS)
            return keyboard.key_down.invoke(key);
        if (action == GLFW_RELEASE)
            return keyboard.key_up.invoke(key);

        // action can also be GLFW_REPEAT (ignored!!)
    });

    // window
    glfwSetWindowSizeCallback(win, [](GLFWwindow* glfw_win, int x, int y){
        static_cast<Window*>(glfwGetWindowUserPointer(glfw_win))->resized.invoke(x, y);
    });

    glfwSetWindowPosCallback(win, [](GLFWwindow* glfw_win, int x, int y){
        static_cast<Window*>(glfwGetWindowUserPointer(glfw_win))->moved.invoke(x, y);
    });

    glfwSetWindowIconifyCallback(win, [](GLFWwindow* glfw_win, int){
        static_cast<Window*>(glfwGetWindowUserPointer(glfw_win))->minimized.invoke();
    });

    glfwSetWindowMaximizeCallback(win, [](GLFWwindow* glfw_win, int){
        static_cast<Window*>(glfwGetWindowUserPointer(glfw_win))->maximized.invoke();
    });

    glfwSetWindowCloseCallback(win, [](GLFWwindow* glfw_win){
        static_cast<Window*>(glfwGetWindowUserPointer(glfw_win))->closed.invoke();
    });
}