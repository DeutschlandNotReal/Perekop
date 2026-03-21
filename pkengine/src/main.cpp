#include "gl.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include "engine.hpp"

const float iFPS = 1.0f / 60.0f;

namespace PKENGINE {
    static PKEvent<float> render;
    static PKEvent<float> frame;
}

void nextFrame() {

}

void nextTick() {
    
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


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
     
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}