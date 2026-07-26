#define PK_INTERNAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <PK/Internal.hpp>
using namespace pk;

// Mouse::pos ranges from 0 to 1 where (1, 1) is the top-right corner
// glfw does it in pixels and where (1, 1) is bottom-right corner...

f64 mouse_lx, mouse_ly;
namespace Perekop::Mouse {
    quat rot() noexcept {
        
    }

    bool held(Button b) {
        return glfwGetMouseButton(glfw_window, b) == 1;
    }

    void point_to(vec2 p) {
        mouse_lx = p.x; mouse_ly = p.y;
        glfwSetCursorPos(glfw_window, mouse_lx, mouse_ly);
    }

    void lock() { glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
    void unlock() { glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }
    bool is_locked() { return glfwGetInputMode(glfw_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED; }
}

namespace Perekop::Input {
    bool held(int k) {
        return glfwGetKey(glfw_window, k);
    }
}

namespace Perekop::Window {
    vec2 get_size() noexcept {
        i32 x, y; 
        glfwGetWindowSize(glfw_window, &x, &y); return {x, y};
    }

    void set_size(vec2 size) noexcept { glfwSetWindowSize(glfw_window, size.x, size.y); }
    void set_title(strview title) noexcept { glfwSetWindowTitle(glfw_window, title); }

    void minimize() noexcept { glfwIconifyWindow(glfw_window); }
    void maximize() noexcept { glfwMaximizeWindow(glfw_window); }
}

void Perekop::init_window() {
    int winwidth, winheight;
    glfwGetWindowSize(glfw_window, &winwidth, &winheight);

    glfwGetCursorPos(glfw_window, &mouse_lx, &mouse_ly);
    Mouse::pos = vec2{mouse_lx, winheight - mouse_ly} / vec2{winwidth, winheight};

    glfwSetMouseButtonCallback(glfw_window, [](GLFWwindow*, int k, int act, int){
        switch (act) {
            case GLFW_PRESS: 
                return Mouse::on_down.fire(Mouse::Button(k));
            case GLFW_RELEASE: 
                return Mouse::on_up.fire(Mouse::Button(k));
        }
    }); 

    glfwSetScrollCallback(glfw_window, [](GLFWwindow*, double x, double y){
        Mouse::on_scroll.fire(y);
    });

    glfwSetCursorPosCallback(glfw_window, [](GLFWwindow*, double x, double y){
        if (x == mouse_lx && y == mouse_ly) return;
        double dx = x - mouse_lx, dy = y - mouse_ly;
        vec2 size = Window::get_size();

        vec2 delta = vec2{dx, dy} / size;
        Perekop::query_gui();

        mouse_lx = x; mouse_ly = y;
        if (!Mouse::is_locked()) Mouse::pos -= delta;
   
        Mouse::on_move.fire(delta);
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
        Window::on_resize.fire({w, h});
    });
}

void Perekop::query_gui() {
    Gui::top = nullptr;

    for (gui_instance &gui : Gui::items) {
        gui.entered = gui.is_intersecting(Mouse::pos);
        if (gui.entered && (!Gui::top || Gui::top->Z > gui.Z))
            Gui::top = &gui; 
    }
}