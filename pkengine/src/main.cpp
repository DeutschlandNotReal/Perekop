#include "Perekop/Mesh.hpp"
#include <exception>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>   
#include <iostream>

#include <Perekop/Engine.hpp>
#include <Perekop/Time.hpp>

using glm::vec3, glm::vec2; 
using namespace pk;
using namespace Perekop;

#define EVENT(N, ...) static Event<__VA_ARGS__> N##_event = Event<__VA_ARGS__>(); EventPort<__VA_ARGS__>& N = N##_event.port;
namespace Perekop {
    namespace Window {
        float FPS = 60;
        static GLFWwindow* win;

        EVENT(step, double)
        EVENT(resized, int, int);
        
        glm::vec2 get_size() { 
            int x, y;
            glfwGetWindowSize(win, &x, &y);
            return glm::vec2(x, y);
        }
        void get_size(int& x, int& y) {
            glfwGetWindowSize(win, &x, &y);
        }
        void set_size(int x, int y) {
            glfwSetWindowSize(win, x, y);
        }
        void set_size(glm::vec2 d) {
            set_size(d.x, d.y);
        }
        std::string get_title() {
            return std::string(glfwGetWindowTitle(win));
        }
        void set_title(const std::string& title) {
            glfwSetWindowTitle(win, title.c_str());
        }
        void maximize() {
            glfwMaximizeWindow(win);
        }
        void minimize() {
            glfwIconifyWindow(win);
        }
        void close() {
            glfwSetWindowShouldClose(win, GLFW_TRUE);
        }
    }
    namespace Mouse {
        glm::vec2 pos() { 
            double x, y;
            glfwGetCursorPos(Window::win, &x, &y);
            return glm::vec2(x, y);
        }
        glm::vec2 normalized_pos() {
            double x, y;
            int w, h;
            glfwGetCursorPos(Window::win, &x, &y);
            glfwGetWindowSize(Window::win, &w, &h);
            return glm::vec2(x/(double)w, y/(double)h);
        }
        void set_pos(glm::vec2 pos) {
            glfwSetCursorPos(Window::win, pos.x, pos.y);
        }
        void set_normalized_pos(glm::vec2 pos) {
            glm::vec2 size = Window::get_size();
            glfwSetCursorPos(Window::win, pos.x * size.x, pos.y * size.y);
        }

        EVENT(rmb_down)
        EVENT(rmb_up)
        EVENT(lmb_down)
        EVENT(lmb_up)
        EVENT(move, double, double)
        bool is_rmb_down() { return glfwGetMouseButton(Window::win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS; }
        bool is_lmb_down() { return glfwGetMouseButton(Window::win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS; }
    }
    namespace Input {
        EVENT(key_down, int)
        EVENT(key_up, int)

        std::string get_clipboard() { return std::string(glfwGetClipboardString(Window::win)); }
        void set_clipboard(const std::string& content) { glfwSetClipboardString(Window::win, content.c_str()); }
        bool is_key_down(int key) { return glfwGetKey(Window::win, key) == GLFW_PRESS; }
    }
    namespace Scene {
        static MeshMaterial default_mat = MeshMaterial();
        static MeshRenderer Renderer = MeshRenderer();
        Camera camera = Camera();
        Mesh* new_mesh(MeshMaterial material) {
            return Renderer.create_mesh(material); 
        }
        Mesh* new_mesh() {
            return Renderer.create_mesh(default_mat); 
        }
    }
};
#undef EVENT

int main() {
    std::cout << "begin?\n";
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Window::win = glfwCreateWindow(400, 800, "Perekop", NULL, NULL);
    glfwMakeContextCurrent(Window::win);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { return -1; }

    Scene::default_mat = MeshMaterial(
        "void main() { gl_Position = VP * model() * vec4(_pos, 1.0); }",
        "void main() { fragColor = vec4(1.0, 0.0, 0.0, 1.0); }"
    ); 

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glfwShowWindow(Window::win);

    glfwSetFramebufferSizeCallback(Window::win, [](GLFWwindow*, int x, int y){
        if (x+y == 0) return; // whole thing mysteriously crashes when its (0, 0)
        glViewport(0, 0, x, y);
        Window::resized_event.invoke(x, y);
    });

    glfwSetCursorPosCallback(Window::win, [](GLFWwindow*, double x, double y){
        Mouse::move_event.invoke(x, y);
    });

    glfwSetKeyCallback(Window::win, [](GLFWwindow*, int key, int act, int, int){
        if (act == GLFW_PRESS) { Input::key_down_event.invoke(key); return; }
        if (act == GLFW_RELEASE) { Input::key_up_event.invoke(key); return; }
    });

    glfwSetMouseButtonCallback(Window::win, [](GLFWwindow*, int button, int act, int){
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (act == GLFW_PRESS) Mouse::lmb_down_event.invoke();
            if (act == GLFW_RELEASE) Mouse::lmb_up_event.invoke();
            return;
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (act == GLFW_PRESS) Mouse::rmb_down_event.invoke();
            if (act == GLFW_RELEASE) Mouse::rmb_down_event.invoke();
        }
    });

    try { Game::launch(); } catch (const std::exception &e) {
        std::cout << "launch() thrown error: " << e.what() << "\n";
        return -1;
    }

    StackTimer<double, 3> frame_timer;
    double last_time = frame_timer.now();
    while (true) {
        double dt = frame_timer.delta(last_time);
        glm::vec2 size = Window::get_size();
        glfwPollEvents();
        frame_timer.push();
        if (glfwWindowShouldClose(Window::win)) break;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        if (size.x + size.y > 0) Scene::Renderer.draw();
        Window::step_event.invoke(dt);
        glfwSwapBuffers(Window::win);
        double ftime = frame_timer.pop();
        
        frame_timer.sleep((double)(1 / Window::FPS) - ftime);
    }
    try { Game::close(); } catch (const std::exception &e) {
        std::cout << "close() thrown error: " << e.what() << "\n";
    }

    glfwTerminate();
}