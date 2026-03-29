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
*/

const double iFPS = 1.0 / 60.0;

using glm::vec3, glm::vec2; 
using namespace pk;

#define load_event(N, ...) static Event<__VA_ARGS__> N##_event = Event<__VA_ARGS__>(); const EventPort<__VA_ARGS__>& N = N##_event.port;
namespace pk::engine {
    static MeshRenderer renderer = MeshRenderer();
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

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win = glfwCreateWindow(400, 800, "Perekop", NULL, NULL);
    glfwMakeContextCurrent(win);

    //if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { return -1; }

    engine::window::began_event.invoke();
    engine::window::began_event.freeze();
    double last_dt = iFPS;

    util::StackTimer<double, 10> timer{};

    while (!glfwWindowShouldClose(win)) {
        timer.begin();
        glfwPollEvents();

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

        std::cout << "Render: " << dt_render << ", Event: " << dt_event;

        timer.sleep(iFPS - dt);
        last_dt = dt;
    }
    
    engine::window::ended_event.invoke();
    engine::window::ended_event.freeze();

    glfwTerminate();
}