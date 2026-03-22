#include "gl.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include "engine.hpp"

#include <thread>
#include <chrono>

const float iFPS = 1.0f / 60.0f;

namespace PKENGINE {
    static PKRenderer renderer;
    static pk::event<float> frame_stepped;
    static pk::event<float> tick_stepped;
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
    PKENGINE::frame_stepped = pk::event<float>();
    PKENGINE::tick_stepped = pk::event<float>();
    PKENGINE::renderer = PKRenderer();

    pk::engine::events::frame_stepped = PKENGINE::frame_stepped.port();
    pk::engine::events::tick_stepped = PKENGINE::tick_stepped.port();

    // where the good stuff happens
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
     
        PKENGINE::frame_stepped.invoke(iFPS);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}