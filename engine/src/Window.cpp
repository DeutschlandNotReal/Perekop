#define PK_INTERNAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <PK/Internal.hpp>
using namespace pk;

// Mouse::pos ranges from 0 to 1 where (1, 1) is the top-right corner
// glfw does it in pixels and where (1, 1) is bottom-right corner...

vec2 lpos;
namespace Perekop::Mouse {
    extern const pk::transform t{};

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

    f64 mx, my;
    glfwGetCursorPos(glfw_window, &mx, &my);
    lpos = {mx, my};
    Mouse::pos = vec2{lpos.x, winheight - lpos.y} / vec2{winwidth, winheight};

    glfwSetMouseButtonCallback(glfw_window, [](GLFWwindow*, i32 k, i32 act, i32){
        switch (act) {
            case GLFW_PRESS: return Mouse::on_down.fire(Mouse::Button(k));
            case GLFW_RELEASE: return Mouse::on_up.fire(Mouse::Button(k));
        }
    }); 

    glfwSetScrollCallback(glfw_window, [](GLFWwindow*, f64 x, f64 y){
        Mouse::on_scroll.fire((i32)y);
    });

    glfwSetCursorPosCallback(glfw_window, [](GLFWwindow*, f64 x, f64 y){
        vec2 pos{x, y};
        if (pos.x == lpos.x && pos.y == lpos.y) return;

        vec2 rsize = 1.f / Window::get_size();
        vec2 delta = (pos - lpos) * rsize; // [-1 -> 1]
        //Perekop::query_gui();


        lpos = pos;
        if (!Mouse::is_locked()) {
            Mouse::pos -= delta;
            f32 anglek = (pk::pi<> / 180.f) * World::camera.fov;
 
            Mouse::transform = World::camera.t * quat::axis_angle({0,1,0}, anglek * delta.x) * quat::axis_angle({1,0,0}, anglek * delta.y);
        }
   
        Mouse::on_move.fire(delta);
    });

    glfwSetKeyCallback(glfw_window, [](GLFWwindow*, i32 k, i32, i32 act, i32){
        switch (act) {
            case GLFW_PRESS: return Input::on_down.fire(k);
            case GLFW_RELEASE: return Input::on_up.fire(k); 
        }
    });

    glfwSetWindowSizeCallback(glfw_window, [](GLFWwindow*, i32 w, i32 h){
        glViewport(0, 0, w, h);
        Window::on_resize.fire({w, h});
    });
}

void Perekop::query_gui() {
    /*Gui::top = nullptr;

    for (gui_instance &gui : Gui::items) {
        gui.entered = gui.is_intersecting(Mouse::pos);
        if (gui.entered && (!Gui::top || Gui::top->Z > gui.Z))
            Gui::top = &gui; 
    */
}