#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>   
#include <iostream>

#include <Perekop/Engine.hpp>
#include <Perekop/Geometry.hpp>
#include <Perekop/Time.hpp>

using glm::vec3, glm::vec2; 
using namespace pk;
using namespace Perekop;

#define load_event(N, ...) static Event<__VA_ARGS__> N##_event = Event<__VA_ARGS__>(); EventPort<__VA_ARGS__>& N = N##_event.port;
namespace Perekop {
    namespace Window {
        float FPS = 60;
        static int screen_x = 100, screen_y = 100;
            static GLFWwindow* win;

        load_event(step, double)
        load_event(resized, int, int)  
    }
    namespace Input {
        load_event(mouse_moved, vec2);
        load_event(input_began, int);
        load_event(input_ended, int);
    }
    namespace Scene {
        static MeshRenderer Renderer = MeshRenderer();
        Camera camera = Camera();
        Mesh* new_mesh() {
            return Renderer.create_mesh(); 
        }
    }
};
#undef load_event

void on_resize(GLFWwindow* win, int x, int y) {
    Window::screen_x = x; Window::screen_y = y;
    if (x+y == 0) return; // whole thing mysteriously crashes when its (0, 0)
    glViewport(0, 0, x, y);
    Window::resized_event.invoke(x, y);
}

glm::vec2 Window::get_size() {
    return glm::vec2(Window::screen_x, Window::screen_y);
}

void glfw_error(int error, const char* lore) {
    std::cerr << "glfw error " << error << ": " << lore << "\n";
}

int main() {
    std::cout << "begin?\n";
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwSetErrorCallback(glfw_error);

    Window::win = glfwCreateWindow(400, 800, "Perekop", NULL, NULL);
    glfwMakeContextCurrent(Window::win);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { return -1; }

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glfwShowWindow(Window::win);

    Game::launch();
    MeshRenderer::init();

    glfwSetFramebufferSizeCallback(Window::win, on_resize);

    StackTimer<double, 3> frame_timer;
    double dt = 0;
    while (true) {
        frame_timer.push();
        glfwPollEvents();
        if (glfwWindowShouldClose(Window::win)) break;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        if (Window::screen_x + Window::screen_y > 0) {
            Scene::Renderer.draw(Scene::camera, Window::screen_x, Window::screen_y);
        }

        Window::step_event.invoke(dt);
        glfwSwapBuffers(Window::win);

        frame_timer.pop(dt);
        frame_timer.sleep((double)(1 / Window::FPS) - dt);
    }
    Game::close();

    glfwTerminate();
}