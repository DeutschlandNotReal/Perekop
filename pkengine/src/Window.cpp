#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Internal.hpp>
using namespace pk;
using namespace glm;

namespace Perekop::Mouse {
    vec2 position() {
        double x, y;
        glfwGetCursorPos(glfw_window, &x, &y);
        return {x, y};
    }

    void position(vec2 p) {
        glfwSetCursorPos(glfw_window, p.x, p.y);
    }

    vec3 fvec() {
        vec2 m = position(), s = Window::size();
        vec2 ndc = {(2.f*m.x/s.x)-1.f, 1.f-(2.f*m.y/s.y)};
        float ar = s.x/s.y, t = tan(radians(World::camera.fov)*0.5f);
        return normalize(World::camera.pose.wspace_vec({ndc.x*ar*t, ndc.y*t, -1}));
    }

    mat3 matrix() {
        vec3 
        f = fvec(),
        r = normalize(cross(f, vec3{0,1,0})),
        u = normalize(cross(r, f));
        return {r, u, f};
    }

    bool held(int b) {
        return glfwGetMouseButton(glfw_window, b) == 1;
    }

    void set(State state) {
        glfwSetInputMode(glfw_window, GLFW_CURSOR, state);
    }
}

namespace Perekop::Input {
    bool held(int k) {
        return glfwGetKey(glfw_window, k);
    }
}

namespace Perekop::Window {
    void size(vec2 s) {
        glfwSetWindowSize(glfw_window, s.x, s.y);
    }
    vec2 size() { 
        int x, y;
        glfwGetWindowSize(glfw_window, &x, &y);
        return {x, y};
    }
    void position(vec2 p) {
        glfwSetWindowPos(glfw_window, p.x, p.y);
    }
    vec2 position() {
        int x, y;
        glfwGetWindowPos(glfw_window, &x, &y);
        return {x, y};
    }
    void title(const char* t) {
        glfwSetWindowTitle(glfw_window, t);
    }
    const char* title() { return glfwGetWindowTitle(glfw_window); }
    void minimize() { glfwIconifyWindow(glfw_window); }
    void maximize() { glfwMaximizeWindow(glfw_window); }
}

double lx{0}, ly{0};
void Perekop::init::listeners() {
    glfwGetCursorPos(glfw_window, &lx, &ly);
    glfwSetMouseButtonCallback(glfw_window, [](GLFWwindow*, int k, int act, int){
        switch (act) {
            case GLFW_PRESS: return Mouse::on_down.fire(k);
            case GLFW_RELEASE: return Mouse::on_up.fire(k);
        }
    }); 

    glfwSetScrollCallback(glfw_window, [](GLFWwindow*, double x, double y){
        Mouse::on_scroll.fire(y);
    });

    glfwSetCursorPosCallback(glfw_window, [](GLFWwindow*, double x, double y){
        vec2 delta = {x-lx, y-ly};
        lx = x; ly = y;
        Mouse::on_move.fire(delta);
    });

    glfwSetKeyCallback(glfw_window, [](GLFWwindow*, int k, int, int act, int){
        switch (act) {
            case GLFW_PRESS: return Input::on_down.fire(k);
            case GLFW_RELEASE: return Input::on_up.fire(k); 
        }
    });

    glfwSetWindowSizeCallback(glfw_window, [](GLFWwindow*, int w, int h){
       glViewport(0, 0, w, h); 
    });
}
