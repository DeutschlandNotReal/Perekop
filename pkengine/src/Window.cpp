#include <pk/Window.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
using namespace pk;
using namespace glm;

using Input = Window::Input;

inline Window& fetch(GLFWwindow* ptr) {
    return *static_cast<Window*>(glfwGetWindowUserPointer(ptr));
}

vec2 Input::pos() const {
    double x, y; glfwGetCursorPos(ptr, &x, &y);
    return {x, y};
}

void Input::pos(vec2 p) {
    lpos = p;
    glfwSetCursorPos(ptr, p.x, p.y);
}

void Input::mouse_mode(Input::CursorMode mode) const {
    glfwSetInputMode(ptr, GLFW_CURSOR, mode);
}

bool Input::is_held_k(int k) const {
    return glfwGetKey(ptr, k) == GLFW_PRESS;
}

bool Input::is_held_m(int b) const {
    return glfwGetMouseButton(ptr, b) == GLFW_PRESS;
}

glm::vec2 Window::pos() const {
    int x, y;
    glfwGetWindowPos(ptr, &x, &y); return {x, y};
}

void Window::pos(glm::vec2 p) const {
    glfwSetWindowPos(ptr, p.x, p.y);
}

glm::vec2 Window::size() const {
    int x, y; glfwGetWindowSize(ptr, &x, &y);
    return {x, y};
}

void Window::size(glm::vec2 s) const {
    glfwSetWindowSize(ptr, s.x, s.y);
}

const char* Window::title() const { 
    return glfwGetWindowTitle((GLFWwindow*)ptr); 
}

void Window::title(const char* t) const { 
    glfwSetWindowTitle((GLFWwindow*)ptr, t); 
}


void Window::maximize() const { glfwMaximizeWindow(ptr); }
void Window::minimize() const { glfwIconifyWindow(ptr); }
void Window::close() const{ glfwDestroyWindow(ptr); }
void Window::attention() const { glfwRequestWindowAttention(ptr); }
void Window::focus() const { glfwFocusWindow(ptr); }
void Window::set_context() const { glfwMakeContextCurrent(ptr); }
void Window::show() const { glfwShowWindow(ptr); }
void Window::swap() const { glfwSwapBuffers(ptr); }
bool Window::should_close() const { return glfwWindowShouldClose(ptr); }
bool Window::visible() const { return glfwGetWindowAttrib(ptr, GLFW_VISIBLE); }

Window::Window(const char* title, int w, int h):
    ptr(glfwCreateWindow(w, h, title, NULL, NULL)) {
    glfwSetWindowUserPointer(ptr, this);

    glfwSetCursorPosCallback(ptr, [](GLFWwindow* ptr, double x, double y){
        Window& self = fetch(ptr);
        self.input.on_move_m.fire(vec2{x, y} - self.input.lpos);
        self.input.lpos = {x, y};  
    });

    glfwSetMouseButtonCallback(ptr, [](GLFWwindow* ptr, int button, int action, int){
        Window& self = fetch(ptr);
        switch (action) {
            case GLFW_PRESS: 
                return self.input.on_down_m.fire(button);
            case GLFW_RELEASE: 
                return self.input.on_up_m.fire(button);
            case GLFW_REPEAT:
                break; // IGNORED!!
        };
    });

    glfwSetScrollCallback(ptr, [](GLFWwindow* ptr, double, double n){
        fetch(ptr).input.on_scroll_m.fire(n);
    });

    glfwSetKeyCallback(ptr, [](GLFWwindow* ptr, int key, int, int action, int){
        Window& self = fetch(ptr);
        switch (action) {
            case GLFW_PRESS:
                return self.input.on_down_k.fire(key);
            case GLFW_RELEASE:
                return self.input.on_up_k.fire(key);
            case GLFW_REPEAT:
                break; // IGNORED!!
        }
    });

    glfwSetWindowSizeCallback(ptr, [](GLFWwindow* ptr, int x, int y){
        glViewport(0, 0, x, y);
        fetch(ptr).on_resize.fire(x, y);
    });

    glfwSetWindowPosCallback(ptr, [](GLFWwindow* ptr, int x, int y){
        fetch(ptr).on_move.fire(x, y);
    });

    glfwSetWindowIconifyCallback(ptr, [](GLFWwindow* ptr, int){
        fetch(ptr).on_minimize.fire();
    });

    glfwSetWindowMaximizeCallback(ptr, [](GLFWwindow* ptr, int){
        fetch(ptr).on_maximize.fire();
    });

    glfwSetWindowCloseCallback(ptr, [](GLFWwindow* ptr){
        fetch(ptr).on_close.fire();
    });
}