#define PK_INTERNAL
#include <format>
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <PK/Internal.hpp>
#include <PK/Util/time.hpp>
#include <PK/Util/file.hpp>

using namespace pk;
using namespace Perekop;

void Perekop::exit() { glfwDestroyWindow(glfw_window); }

i32 main() {
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

    time::Tracker<f64, 2> frame_timer;
    frame_timer.begin();
    printf("on_launch() begin\n");
    on_launch();
    printf("on_launch() end (%.2fms)\n", frame_timer.stop() * 1000);

    frame_timer.begin();
    glfwSetWindowRefreshCallback(glfw_window, [](GLFWwindow*){
        Perekop::render(false);
    });

    f64 accumulator{0};

    f64 rfps = 1.0 / World::fps;
    while (!glfwWindowShouldClose(glfw_window)) {
        accumulator += frame_timer.delta();

        if (accumulator >= rfps) {
            glfwPollEvents();
            i32 ticks = accumulator * World::fps;
    
            if (ticks > 4) {
                // ticks dropped to not overload
                accumulator -= (ticks - 2) * rfps;
                ticks = 2;
            }

            frame_timer.begin();
            while (accumulator >= rfps) {
                accumulator -= rfps;
                step_physics(rfps);
                on_step(rfps);
            }

            f64 util = frame_timer.stop() * World::fps * 100;

            glfwSetWindowTitle(glfw_window, std::format("Perekop | UTIL {:2.3f}%", util).c_str());

            render(true);
        }

        std::this_thread::yield();
    }
    frame_timer.stop();
    frame_timer.begin();

    printf("on_exit() begin\n");
    on_exit();
    printf("on_exit() end (%.2fms)\n", frame_timer.stop() * 1000);

    glfwTerminate();
}