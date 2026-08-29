#define PK_INTERNAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <PKINT/internal.hpp>
using namespace pk;

namespace Perekop::Mouse {
    namespace {
        inline listeners<Button> OnPress;
        inline listeners<Button> OnRelease;
        inline listeners<vec2>   OnMove;
        inline listeners<float>  OnScroll;

        inline vec2 lastpos;
    };

    bool Held(Button b) noexcept {
        return glfwGetMouseButton(glfw_window, b) == 1;
    }

    void SetPosition(vec2 p) noexcept {
        lastpos = p;
        glfwSetCursorPos(glfw_window, p.x, p.y);
    }

    void Lock() noexcept { 
        glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
    }

    void Unlock() noexcept {
        glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); 
    }

    bool Locked() noexcept {
        return glfwGetInputMode(glfw_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED; 
    }

    ImplementBinder(BindToMove, OnMove, vec2);
    ImplementBinder(BindToScroll, OnScroll, float);
    ImplementBinder(BindToPress, OnPress, Button);
    ImplementBinder(BindToRelease, OnRelease, Button);
}

namespace Perekop::Input {
    namespace {
        inline listeners<int> OnPress;
        inline listeners<int> OnRelease;
    }

    bool Held(int k) noexcept {
        return glfwGetKey(glfw_window, k);
    }

    ImplementBinder(BindToPress, OnPress, int);
    ImplementBinder(BindToRelease, OnRelease, int);
}

namespace Perekop::Window {
    namespace {
        inline listeners<vec2> OnResize; 
    }

    vec2 Size() noexcept {
        i32 x, y; 
        glfwGetWindowSize(glfw_window, &x, &y); return {x, y};
    }

    void SetSize(vec2 size) noexcept { 
        glfwSetWindowSize(glfw_window, size.x, size.y); 
    }

    void SetTitle(std::string_view title) noexcept { 
        glfwSetWindowTitle(glfw_window, title.begin()); 
    }

    void Minimize() noexcept { glfwIconifyWindow(glfw_window); }
    void Maximize() noexcept { glfwMaximizeWindow(glfw_window); }

    ImplementBinder(BindToResize, OnResize, vec2);
}

void Perekop::WindowBegin() noexcept {
    int winwidth, winheight;
    glfwGetWindowSize(glfw_window, &winwidth, &winheight);

    double mx, my;
    glfwGetCursorPos(glfw_window, &mx, &my);
    Mouse::lastpos = {mx, my};

    glfwSetMouseButtonCallback(glfw_window, [](GLFWwindow*, int k, int act, int){
        switch (act) {
            case GLFW_PRESS: 
                return InvokeListeners(Mouse::OnPress, Mouse::Button(k));
            case GLFW_RELEASE:
                return InvokeListeners(Mouse::OnRelease, Mouse::Button(k));
        }
    }); 

    glfwSetScrollCallback(glfw_window, [](GLFWwindow*, double x, double y){
        InvokeListeners(Mouse::OnScroll, (float)y);
    });

    glfwSetCursorPosCallback(glfw_window, [](GLFWwindow*, double x, double y){
        vec2 pos{x, y};
        if (pos == Mouse::lastpos) return;
        vec2 delta = pos - std::exchange(Mouse::lastpos, pos);

        InvokeListeners(Mouse::OnMove, (vec2)delta);
    });

    glfwSetKeyCallback(glfw_window, [](GLFWwindow*, int k, int, int act, int){
        switch (act) {
            case GLFW_PRESS: 
                return InvokeListeners(Input::OnPress, (int)k);
            case GLFW_RELEASE:
                return InvokeListeners(Input::OnRelease, (int)k);
        }
    });

    glfwSetWindowSizeCallback(glfw_window, [](GLFWwindow*, int w, int  h){
        glViewport(0, 0, w, h);
        InvokeListeners(Window::OnResize, vec2{w, h});
    });
}