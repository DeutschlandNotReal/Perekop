#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>   
#include <iostream>

#include <Perekop/Engine.hpp>
#include <Perekop/Time.hpp>
#include <thread>

using glm::vec3, glm::vec2; 
using namespace pk;
using namespace Perekop;


#define EVENT(N, ...) static Event<__VA_ARGS__> N##_event = Event<__VA_ARGS__>(); EventPort<__VA_ARGS__>& N = N##_event.port;
namespace Perekop::Window {
    float FPS = 60;
    static GLFWwindow* win;

    EVENT(step, double)
    EVENT(resized, int, int);
      
    glm::vec2 size() { 
        int x, y;
        glfwGetWindowSize(win, &x, &y);
       return glm::vec2(x, y);
    }
    void set_size(glm::vec2 d) {
        glfwSetWindowSize(win, d.x, d.y);
    }
    std::string title() {
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

namespace Perekop::Mouse {
    glm::vec2 pos() { 
        double x, y;
        glfwGetCursorPos(Window::win, &x, &y);
        return glm::vec2(x, y);
    }

    void set_pos(glm::vec2 pos) {
        glfwSetCursorPos(Window::win, pos.x, pos.y);
    }

    EVENT(right_pressed)
    EVENT(right_released)
    EVENT(left_pressed)
    EVENT(left_released)
    EVENT(move, double, double)
    bool right_down() { return glfwGetMouseButton(Window::win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS; }
    bool left_down() { return glfwGetMouseButton(Window::win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS; }
}

namespace Perekop::Input {
    EVENT(key_pressed, int)
    EVENT(key_released, int)

    std::string get_clipboard() { return std::string(glfwGetClipboardString(Window::win)); }
    void set_clipboard(const std::string& content) { glfwSetClipboardString(Window::win, content.c_str()); }
    bool key_down(int key) { return glfwGetKey(Window::win, key) == GLFW_PRESS; }
}

namespace Perekop::Scene {
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

#undef EVENT

static pk::StackTimer<double, 5> timer;

double draw() {
    double dt = timer.delta();
    glm::vec2 size = Window::size();
    timer.push();

    if (size.x + size.y > 0) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Scene::Renderer.draw(); glfwSwapBuffers(Window::win);
    }

    glfwPollEvents();
    Window::step_event.invoke(dt);
    return (1.0/Window::FPS) - timer.pop();
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Window::win = glfwCreateWindow(720, 400, "Perekop", NULL, NULL);
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
        if (act == GLFW_PRESS) return Input::key_pressed_event.invoke(key);
        if (act == GLFW_RELEASE) return Input::key_released_event.invoke(key);
    });

    glfwSetMouseButtonCallback(Window::win, [](GLFWwindow*, int button, int act, int){
        using namespace Mouse;
        switch (button | (act << 8)) {
            case GLFW_MOUSE_BUTTON_LEFT | GLFW_PRESS << 8:  return left_pressed_event.invoke();
            case GLFW_MOUSE_BUTTON_LEFT | GLFW_RELEASE << 8: return left_released_event.invoke();
            case GLFW_MOUSE_BUTTON_RIGHT | GLFW_PRESS << 8: return right_pressed_event.invoke();
            case GLFW_MOUSE_BUTTON_RIGHT | GLFW_RELEASE << 8: return right_released_event.invoke();
        }
    });

    Game::launch();
    timer.push();

    while (!glfwWindowShouldClose(Window::win)) {
        double frametime_left = draw();

        if (frametime_left > 0) {
            timer.sleep(frametime_left);
        } else {
            std::this_thread::yield();
        }
    }

    Game::close();
    glfwTerminate();
}