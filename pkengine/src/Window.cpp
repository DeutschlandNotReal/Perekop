#include "Perekop.hpp"
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Internal.hpp>
using namespace pk;
using namespace glm;

namespace Perekop::Mouse {
    vec3 fvec() {
        vec2 ndc = {(2.f*pos.x/Window::size.x)-1.f, (2.f*pos.y/Window::size.y)-1.f};
        float t = tan(radians(World::camera.fov)*.5f);

        return normalize(
            World::camera.pose.wspace_vec({
                ndc.x * t * (Window::size.x / Window::size.y),
                ndc.y * t,
                -1
            })
        );
    }

    mat3 matrix() {
        vec3 
            f = fvec(),
            r = normalize(cross(f, vec3{0,1,0})),
            u = normalize(cross(r, f));
        return {r, u, f};
    }

    bool held(Button b) {
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
    void minimize() { glfwIconifyWindow(glfw_window); }
    void maximize() { glfwMaximizeWindow(glfw_window); }
}

void Perekop::init::window() {
    {
        int w, h;
        glfwGetWindowSize(glfw_window, &w, &h);
        Window::size = Window::size_v = {w, h};
    }{
        int x, y;
        glfwGetWindowPos(glfw_window, &x, &y);
        Window::pos = Window::pos_v = {x, y};
    }
    {
        double x, y;
        glfwGetCursorPos(glfw_window, &x, &y);
        Mouse::pos = Mouse::pos_v = {x, Window::size.y - y};
    }

    glfwSetMouseButtonCallback(glfw_window, [](GLFWwindow*, int k, int act, int){
        switch (act) {
            case GLFW_PRESS: 
                if (GUI::top) GUI::on_down.fire(GUI::top, k);
                return Mouse::on_down.fire(Mouse::Button(k));
            case GLFW_RELEASE: 
                if (GUI::top) GUI::on_up.fire(GUI::top, k);
                return Mouse::on_up.fire(Mouse::Button(k));
        }
    }); 

    glfwSetScrollCallback(glfw_window, [](GLFWwindow*, double x, double y){
        // if (Perekop::gui_test_scroll(y)) return;
        Mouse::on_scroll.fire(y);
    });

    glfwSetCursorPosCallback(glfw_window, [](GLFWwindow*, double x, double y){
        Mouse::pos_v = {x, Window::size_v.y - y};
        Perekop::get_gui_top();
        vec2 delta = Mouse::pos_v - Mouse::pos;
        if (glfwGetInputMode(glfw_window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) 
            Mouse::pos_v = Mouse::pos;
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
        Window::size_v = Window::size = vec2{w, h};
    });

    glfwSetWindowPosCallback(glfw_window, [](GLFWwindow*, int x, int y){
        Window::pos_v = Window::pos = vec2{x, y};
    });

    glfwSetWindowRefreshCallback(glfw_window, [](GLFWwindow*){
        Perekop::step::draw();
    });
}

void Perekop::step::window() {
    if (Window::size != Window::size_v) { 
        Window::size_v = Window::size; 
        glfwSetWindowSize(glfw_window, Window::size.x, Window::size.y); 
    } if (Window::pos != Window::pos_v) { 
        Window::pos_v = Window::pos;  
        glfwSetWindowPos(glfw_window, Window::pos.x, Window::pos.y); 
    } if (Mouse::pos != Mouse::pos_v) {
        Mouse::pos_v = Mouse::pos;
        glfwSetCursorPos(glfw_window, Mouse::pos.x, Window::size.y - Mouse::pos.y);
    } if (Window::title != Window::title_v) {
        Window::title_v = Window::title;
        glfwSetWindowTitle(glfw_window, Window::title);
    }
}