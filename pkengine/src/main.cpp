#include "gl.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include "engine.hpp"

#include <thread>
#include <chrono>

const float iFPS = 1.0f / 60.0f;

using pk::Event, pk::MeshRenderer, glm::vec3, glm::vec2; 
using namespace std::chrono;

namespace PKENGINE {
    static MeshRenderer renderer = MeshRenderer();
    static Event<float> frame_stepped = Event<float>();
    static Event<vec2> frame_resized = Event<vec2>();
    static Event<int> frame_closed = Event<int>(); // int will just be 0
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

    // if glfw or glad just doesnt work we quit like chuds
    // probably breaks game maybe

    {
        using namespace pk::engine::frame;
        closed = std::move(PKENGINE::frame_closed.port());
        resized = std::move(PKENGINE::frame_resized.port());
        stepped = std::move(PKENGINE::frame_stepped.port());
    }
    

    // where the good stuff happens
    auto frame_duration = duration<float>(iFPS);
    while (!glfwWindowShouldClose(window)) {
        auto frame_start = high_resolution_clock::now();
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
     
        PKENGINE::frame_stepped.invoke(iFPS);
        glfwSwapBuffers(window);

        auto frame_length = high_resolution_clock::now() - frame_start;

        // will probably sleep for negative time
        // time travel?
        std::this_thread::sleep_for(frame_duration - frame_length);
    }
    // anythign after here is when window closed
    PKENGINE::frame_closed.invoke(0);

    glfwTerminate();
    return 0;
}