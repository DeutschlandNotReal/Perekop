#include "gl.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include "engine.hpp"

#include <thread>
#include <chrono>

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
        load_event(float, step)
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

void pk::engine::init() {
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

    // if glad or glew doesn't work then we're done for
    engine::window::began_event.final(0);
    float last_dt = iFPS;
    while (!glfwWindowShouldClose(window)) {
        auto frame_start = high_resolution_clock::now();
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
     
        engine::window::step_event.invoke(last_dt);
        glfwSwapBuffers(window);

        float this_dt = (high_resolution_clock::now() - frame_start).count();
        last_dt = this_dt;
        if (this_dt < iFPS) std::this_thread::sleep_for(duration<float>(iFPS - this_dt));
    }
    // anything after here for when window closes
    engine::window::ended_event.final(0);

    glfwTerminate();
}