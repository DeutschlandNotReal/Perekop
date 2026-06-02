#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memoryapi.h>

#include <Internal.hpp>
#include <pkutil/Time.hpp>
#include <pkutil/File.hpp>
using namespace pk;
using namespace glm;
using namespace Perekop;

static Shader default_shader;

void Perekop::exit() { glfwDestroyWindow(glfw_window); }

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfw_window = glfwCreateWindow(720, 480, "Perekop", nullptr, nullptr);
    glfwMakeContextCurrent(glfw_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(0);
    glfwShowWindow(glfw_window);
    glEnable(GL_DEPTH_TEST);

    init_render();
    init_window();
    on_launch();

    Time::Tracker<double, 1> ftimer; ftimer.begin();
    while (!glfwWindowShouldClose(glfw_window)) {
        double dt = ftimer.delta();
        glfwPollEvents();

        step_physics(dt);
        render(true);
        step_window();

        on_step(dt);
        double fdt = ftimer.elapsed();
        Time::sleep(1.0/World::fps - fdt);
    } 
    
    on_exit();
    glfwTerminate();
}