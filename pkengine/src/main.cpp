#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>       
#include <iostream>

#include "engine.hpp"
#include "geometry.hpp"
#include "util.hpp"

/*
Remove-Item -Recurse -Force build
cmake -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER="C:/msys64/mingw64/bin/g++.exe" -DCMAKE_C_COMPILER="C:/msys64/mingw64/bin/gcc.exe"
cmake --build build -- -j4
.\build\perekop.exe

*/

const double iFPS = 1.0 / 60.0;

using glm::vec3, glm::vec2; 
using namespace pk;

#define load_event(N, ...) static Event<__VA_ARGS__> N##_event = Event<__VA_ARGS__>(); EventPort<__VA_ARGS__>& N = N##_event.port;
namespace pk::engine {
    static MeshRenderer renderer = MeshRenderer();
    static GLFWwindow* win;

    namespace window {
        static glm::vec2 size;
        load_event(step, double)
        load_event(resized, vec2)
    }
    namespace input {
        load_event(mouse_moved, vec2);
        load_event(input_began, int);
        load_event(input_ended, int);
    }
    namespace scene {
        const Camera camera = Camera();
        Mesh* new_mesh() {
            return renderer.create_mesh(); 
        }
    }
};
#undef load_event

void on_resize(GLFWwindow* win, int x, int y) {
    glm::vec2 size(x, y);
    engine::window::size = size;
    engine::window::resized_event.invoke(size);
}

glm::vec2 engine::window::get_size() {
    int w, h;
    glfwGetWindowSize(engine::win, &w, &h);
    return glm::vec2(w, h);
}

void glfw_error(int error, const char* description) {
    std::cerr << "glfw error " << error << ": " << description << "\n";
}

int main() {
    std::cout << "begin?\n";
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    engine::win = glfwCreateWindow(400, 800, "Perekop", NULL, NULL);
    glfwMakeContextCurrent(engine::win);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { return -1; }

    glfwShowWindow(engine::win);
    glfwSetErrorCallback(glfw_error);

    engine::game::launch();
    double last_dt = iFPS;

    util::StackTimer<double, 10> timer{};

    if (!engine::win) {
        std::cerr << "where the window at\n";
        glfwTerminate();
        return -1;
    }
    MeshRenderer::init();

    glfwSetFramebufferSizeCallback(engine::win, on_resize);
    on_resize(engine::win, 400, 800);

    int frame_count = 0;
    while (true) {
        frame_count++;
        timer.begin();
        glfwPollEvents();
        if (glfwWindowShouldClose(engine::win)) break;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    
        timer.begin();
        engine::renderer.draw(engine::scene::camera);
        double dt_render = timer.stop();
 
        timer.begin();
        // todo: make invokes async
        engine::window::step_event.invoke(last_dt);
        double dt_event = timer.stop();

        double dt = timer.stop();
        glfwSwapBuffers(engine::win);

        timer.sleep(iFPS - dt);
        last_dt = dt;
    }
    engine::game::close();

    glfwTerminate();
}