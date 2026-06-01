#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Internal.hpp>
using namespace pk;
using namespace glm;

double mx, my;
int winx, winy;
int winw, winh;
const char* wint;

// Mouse::pos ranges from 0 to 1 where (1, 1) is the top-right corner
// glfw does it in pixels and where (1, 1) is bottom-right corner...

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

void Perekop::init_window() {
    glfwGetWindowSize(glfw_window, &winw, &winh);
    Window::size = {winw, winh};

    glfwGetWindowPos(glfw_window, &winx, &winy);
    Window::pos = {winx, winy};
    
    glfwGetCursorPos(glfw_window, &mx, &my);
    Mouse::pos = vec2{mx, Window::size.y - my} / Window::size;

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
        Mouse::on_scroll.fire(y);
    });

    glfwSetCursorPosCallback(glfw_window, [](GLFWwindow*, double x, double y){
        if (x == mx && y == my) return;
        vec2 delta = vec2{x, y} - vec2{mx, my};
        Perekop::query_gui();

        if (Mouse::locked) 
            glfwSetCursorPos(glfw_window, mx, my);
        else {
            mx = x; my = y;
        }

        Mouse::pos = vec2{x, Window::size.y - y} / Window::size;
        Mouse::on_move.fire(delta / Window::size);
    });

    glfwSetKeyCallback(glfw_window, [](GLFWwindow*, int k, int, int act, int){
        switch (act) {
            case GLFW_PRESS: 
                return Input::on_down.fire(k);
            case GLFW_RELEASE: 
                return Input::on_up.fire(k); 
        }
    });

    glfwSetWindowSizeCallback(glfw_window, [](GLFWwindow*, int w, int h){
        glViewport(0, 0, w, h);
        if (winw == w && winh == h) return; else { winw = w; winh = h; }
        Window::size = vec2{w, h};
    });

    glfwSetWindowPosCallback(glfw_window, [](GLFWwindow*, int x, int y){
        if (winx == x && winy == y) return; else { winx = x; winy = y; }
        Window::pos = vec2{x, y};
    });

    glfwSetWindowRefreshCallback(glfw_window, [](GLFWwindow*){
        Perekop::render(false);
    });
}

void Perekop::step_window() {
    using namespace Window;
    // checks if game changed window/mouse values, updates if true
    if (size.x != winw || size.y != winh) { 
        winw = size.x, winh = size.y;
        glfwSetWindowSize(glfw_window, winw, winh); 
    } 

    if (pos.x != winw || pos.y != winy) { 
        winx = pos.x; winy = pos.y;
        glfwSetWindowPos(glfw_window, winx, winy);
    }

    vec2 mpos = vec2{mx, size.y - my} / size;
    if (length(mpos - Mouse::pos) > 0.005 && !Mouse::locked) {
        mx = Mouse::pos.x * size.x; my = (1 - Mouse::pos.y) * size.y;
        glfwSetCursorPos(glfw_window, mx, my);
    }

    if (title != wint) {
        wint = title;
        glfwSetWindowTitle(glfw_window, wint);
    }
}