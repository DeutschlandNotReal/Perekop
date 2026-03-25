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

const float iFPS = 1.0f / 60.0f;

using glm::vec3, glm::vec2; 
using namespace std::chrono;
using namespace pk;

// ts creates the event and the port so i dont gotta write same slop
#define load_event(T, N) static Event<T> N##_event = Event<T>(); const EventPort<T>& N = N##_event.port;

namespace pk::engine {
    MeshRenderer mesh_renderer{};
    namespace window {
        static glm::vec2 size;
        load_event(double, step)
        load_event(vec2, resized)
        load_event(bool, began)
        load_event(bool, ended)
    }
    namespace input {
        load_event(vec2, mouse_moved);
        load_event(int, key_pressed);
        load_event(int, key_released);
        load_event(int, mouse_clicked);
        load_event(int, mouse_released);
    }
};
#undef load_event

void on_resize(GLFWwindow* win, int x, int y) {
    glm::vec2 size(x, y);
    engine::window::size = size;
    engine::window::resized_event.invoke_async(size);
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
    engine::window::began_event.lock(0);
    engine::window::ended_event.clear();
    float last_dt = iFPS;

    util::timer timer(10);
    while (!glfwWindowShouldClose(window)) {
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
        if (dt < iFPS) std::this_thread::sleep_for(duration<float>(iFPS - dt));
        last_dt = dt;
    }
    // anything after here for when window closes
    engine::window::ended_event.lock(0);
    engine::window::ended_event.clear();

    glfwTerminate();
}