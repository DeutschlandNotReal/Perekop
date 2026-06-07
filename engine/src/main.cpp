#define PK_ENGINE_SRC
#include <format>
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Internal.hpp>
#include <PKLib/Time.hpp>
#include <PKLib/File.hpp>
using namespace pk;
using namespace glm;
using namespace Perekop;

void Perekop::exit() { glfwDestroyWindow(glfw_window); }

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfw_window = glfwCreateWindow(720, 480, "Perekop", nullptr, nullptr);
    glfwMakeContextCurrent(glfw_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1); // VSYNC!
    glfwShowWindow(glfw_window);
    glEnable(GL_DEPTH_TEST);

    init_render();
    init_window();
    on_launch();

    Time::Tracker<double, 2> frame_timer;
    frame_timer.begin();
    glfwSetWindowRefreshCallback(glfw_window, [](GLFWwindow*){
        Perekop::render(false);
    });

    double accumulator{0};

    while (!glfwWindowShouldClose(glfw_window)) {
        double dt = frame_timer.delta();
        double ifps = 1.0 / World::fps;

        accumulator += dt;
        if (accumulator >= ifps) {
            glfwPollEvents();
            int ticks = accumulator / ifps;

            if (ticks > 4) {
                // ticks dropped to not overload
                accumulator -= (ticks - 1) * ifps;
                ticks = 1;
            }

            frame_timer.begin();
            while (accumulator >= ifps) {
                accumulator -= ifps;
                step_physics(ifps);
                on_step(ifps);
            }
            double utilisation = frame_timer.stop() / ifps;

            glfwSetWindowTitle(glfw_window, std::format("Perekop | UTIL {:2.3f}%", utilisation * 100).c_str());

            render(true);
        }

        std::this_thread::yield();
    }

    on_exit();
    glfwTerminate();
}