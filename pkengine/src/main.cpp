#include <glm/glm.hpp>
#include <iostream>
#include <thread>

#include "engine.hpp"
#include "util.hpp"
#include "window.hpp"

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

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Window main_window(400, 800, "Perekop");
    main_window.make_current();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { return -1; }

    engine::window::began_event.lock();
    double last_dt = iFPS;

    util::StackTimer<double, 10> timer{};

    while (main_window.is_open()) {
        timer.begin();
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    
        timer.begin();
        draw();
        double dt_render = timer.stop();
 
        timer.begin();
        engine::window::step_event.invoke(last_dt);
        double dt_event = timer.stop();

        double dt = timer.stop();

        std::cout << "Render: " << dt_render << ", Event: " << dt_event;

        timer.sleep(iFPS - dt);
        last_dt = dt;
    }
    // anything after here for when window closes
    engine::window::ended_event.lock();

    glfwTerminate();
}