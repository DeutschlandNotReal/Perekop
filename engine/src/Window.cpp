#define PK_INTERNAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <PKINT/internal.hpp>
using namespace pk;

vec2 lpos;
namespace Perekop::Mouse {
    bool held(Button b) {
        return glfwGetMouseButton(glfw_window, b) == 1;
    }

    void point_to(vec2 p) {
        lpos = p;
        glfwSetCursorPos(glfw_window, p.x, p.y);
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

    vec2 size() noexcept {
        i32 x, y; 
        glfwGetWindowSize(glfw_window, &x, &y); return {x, y};
    }

    void size(vec2 size) noexcept { 
        glfwSetWindowSize(glfw_window, size.x, size.y); 
    }

    void title(std::string_view title) noexcept { glfwSetWindowTitle(glfw_window, title.begin()); }

    void minimize() noexcept { glfwIconifyWindow(glfw_window); }
    void maximize() noexcept { glfwMaximizeWindow(glfw_window); }
}

void Perekop::init_window() {
    int winwidth, winheight;
    glfwGetWindowSize(glfw_window, &winwidth, &winheight);

    double mx, my;
    glfwGetCursorPos(glfw_window, &mx, &my);
    lpos = {mx, my};
    Mouse::pos = vec2{lpos.x, winheight - lpos.y} / vec2{winwidth, winheight};

    glfwSetMouseButtonCallback(glfw_window, [](GLFWwindow*, int k, int act, int){
        switch (act) {
            case GLFW_PRESS: return Mouse::on_down.fire(Mouse::Button(k));
            case GLFW_RELEASE: return Mouse::on_up.fire(Mouse::Button(k));
        }
    }); 

    glfwSetScrollCallback(glfw_window, [](GLFWwindow*, double x, double y){
        Mouse::on_scroll.fire((i32)y);
    });

    glfwSetCursorPosCallback(glfw_window, [](GLFWwindow*, double x, double y){
        vec2 pos{x, y};
        if (pos.x == lpos.x && pos.y == lpos.y) return;

        vec2 rsize = 1.f / Window::size();
        vec2 delta = (pos - lpos) * rsize; // [-1 -> 1]
        //Perekop::query_gui();

        lpos = pos;
        if (!Mouse::is_locked()) {
            Mouse::pos -= delta;
            float radfov = radians(World::camera.fov());
 
           Mouse::pose = World::camera.pose * angleAxis(radfov * delta.x, vec3{0, 1, 0}) * angleAxis(radfov * delta.y, vec3{1, 0, 0});
        }
   
        Mouse::on_move.fire(delta);
    });

    glfwSetKeyCallback(glfw_window, [](GLFWwindow*, int k, int, int act, int){
        switch (act) {
            case GLFW_PRESS: return Input::on_down.fire(k);
            case GLFW_RELEASE: return Input::on_up.fire(k); 
        }
    });

    glfwSetWindowSizeCallback(glfw_window, [](GLFWwindow*, int w, int  h){
        glViewport(0, 0, w, h);
        Window::on_resize.fire({w, h});
    });

    Window::size(Window::size()); // aspect init
}

void Perekop::query_gui() {
    /*Gui::top = nullptr;

    for (gui_instance &gui : Gui::items) {
        gui.entered = gui.is_intersecting(Mouse::pos);
        if (gui.entered && (!Gui::top || Gui::top->Z > gui.Z))
            Gui::top = &gui; 
    */
}