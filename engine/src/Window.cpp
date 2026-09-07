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
        inline bool skip_move{false};
    };

    vec2 Position() noexcept {
        double x, y;
        glfwGetCursorPos(glfw_window, &x, &y);
        return {x, y};
    }

    bool Held(Button b) noexcept {
        return glfwGetMouseButton(glfw_window, b) == 1;
    }

    pose GetPose(const pk::Camera& camera) noexcept {
        const vec2 size = Window::Size();
        const vec2 uv = Position() / size * 2.f - 1.f;

        const float aspect = size.x / size.y;
        const float fov = camera.TanFov();

        const vec3 local = normalize(vec3{
            uv.x * fov * aspect,
            -uv.y * fov,
            -1.f
        });

        return pose::LookAt(
            camera.pose.pos,
            worldspace(local, camera.pose),
            camera.pose.Up()
        );
    }

    void SetPosition(vec2 p) noexcept {
        lastpos = p;
        glfwSetCursorPos(glfw_window, p.x, p.y);
    }

    void Lock() noexcept {
        if (Locked()) return;

        vec2 center = Window::Size() * .5f;
        lastpos = center;
        skip_move = true;
        glfwSetCursorPos(glfw_window, center.x, center.y);
        glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void Unlock() noexcept {
        skip_move = false;
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

    glfwSetScrollCallback(glfw_window, [](GLFWwindow*, double, const double y){
        InvokeListeners(Mouse::OnScroll, y);
    });

    glfwSetCursorPosCallback(glfw_window, [](GLFWwindow*, double x, double y){
        vec2 pos{x, y};
        if (Mouse::skip_move) {
            Mouse::lastpos = pos;
            Mouse::skip_move = false;
            return;
        }
        if (pos == Mouse::lastpos) return;
        const vec2 delta = pos - std::exchange(Mouse::lastpos, pos);

        InvokeListeners(Mouse::OnMove, delta);
    });

    glfwSetKeyCallback(glfw_window, [](GLFWwindow*, const int k, int, int act, int){
        switch (act) {
            case GLFW_PRESS: 
                return InvokeListeners(Input::OnPress, k);
            case GLFW_RELEASE:
                return InvokeListeners(Input::OnRelease, k);
        }
    });

    glfwSetWindowSizeCallback(glfw_window, [](GLFWwindow*, int w, int  h){
        glViewport(0, 0, w, h);
        InvokeListeners(Window::OnResize, vec2{w, h});
    });
}