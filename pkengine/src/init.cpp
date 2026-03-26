#include "gl.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include "engine.hpp"
#include "util.hpp"
#include <thread>

/*
Remove-Item -Recurse -Force build
cmake -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER="C:/msys64/mingw64/bin/g++.exe" -DCMAKE_C_COMPILER="C:/msys64/mingw64/bin/gcc.exe"
cmake --build build -- -j4
*/

const double iFPS = 1.0 / 60.0;

using glm::vec3, glm::vec2; 
using namespace std::chrono;
using namespace pk;

#define load_event(N, ...) static Event<__VA_ARGS__> N##_event = Event<__VA_ARGS__>(); const EventPort<__VA_ARGS__>& N = N##_event.port;

namespace pk::engine {
    MeshRenderer mesh_renderer{};
    namespace window {
        static glm::vec2 size;
        load_event(step, double)
        load_event(resized, vec2)
        load_event(began)
        load_event(ended)
    }
    namespace input {
        load_event(mouse_moved, vec2);
        load_event(input_began, int);
        load_event(input_ended, int);
    }
};
#undef load_event

void on_resize(GLFWwindow* win, int x, int y) {
    glm::vec2 size(x, y);
    engine::window::size = size;
    engine::window::resized_event.invoke(size);
}

void engine::init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(410, 670, "Perekop", NULL, NULL);
    if (window == NULL) {
        std::cout << "GLFW not working?!?!\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD not working?!\n";
        return;
    }

    glfwSetFramebufferSizeCallback(window, on_resize);

    // if glad or glew doesn't work then we're done for
    engine::window::began_event.lock();
    double last_dt = iFPS;

    util::timer timer(10);
    while (!glfwWindowShouldClose(window)) {
        // todo: frame events (step) should be async
        timer.begin();
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    
        timer.begin();
        mesh_renderer.draw();
        glfwSwapBuffers(window);
        double dt_render = timer.stop();

        timer.begin();
        engine::window::step_event.invoke(last_dt);
        double dt_event = timer.stop();

        double dt = timer.stop();

        std::cout << "Render: " << dt_render << ", Event: " << dt_event;
        if (dt < iFPS) std::this_thread::sleep_for(duration<double>(iFPS - dt));
        last_dt = dt;
    }
    // anything after here for when window closes
    engine::window::ended_event.lock();

    glfwTerminate();
}