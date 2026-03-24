#include "gl.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include "engine.hpp"

#include <thread>
#include <chrono>

const float iFPS = 1.0f / 60.0f;

using pk::Event, pk::EventPort, pk::MeshRenderer, glm::vec3, glm::vec2; 
using namespace std::chrono;

namespace PKENGINE {
    static MeshRenderer renderer = MeshRenderer();
    static Event<float> window_step = Event<float>();
    static Event<vec2> window_resized = Event<vec2>();
    static Event<bool> window_began = Event<bool>();
    static Event<bool> window_ended = Event<bool>();
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(410, 670, "Perekop", NULL, NULL);
    if (window == NULL) {
        std::cout << "GLFW not working?!?!\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD not working?!\n";
        return -1;
    }

    // if glad or glew doesn't work then we're done for

    {
        using namespace pk::engine::window;
        using namespace PKENGINE;
        const EventPort<bool> began{window_began};
        const EventPort<bool> ended{window_ended};
        const EventPort<float> step{window_step};
        const EventPort<vec2> resized{window_resized};
    }
    

    // where the good stuff happens

    PKENGINE::window_began.invoke_once(0);
    auto frame_time = duration<float>(iFPS);
    while (!glfwWindowShouldClose(window)) {
        auto frame_start = high_resolution_clock::now();
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
     
        PKENGINE::window_step.invoke(iFPS);
        glfwSwapBuffers(window);

        auto free_time = high_resolution_clock::now() - frame_start - frame_time;

        if (free_time.count() > 0) std::this_thread::sleep_for(free_time);
    }
    // anythign after here is when window closed
    PKENGINE::window_ended.invoke_once(0);

    glfwTerminate();
    return 0;
}