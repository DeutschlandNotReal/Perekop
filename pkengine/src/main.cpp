#include <thread>
#define PK_ENGINE_SRC
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Internal.hpp>
#include <pkutil/Time.hpp>
#include <pkutil/File.hpp>
using namespace pk;
using namespace glm;
using namespace Perekop;

void Perekop::exit() { glfwDestroyWindow(glfw_window); }

Time::Tracker<double, 2> frame_timer;

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

    frame_timer.begin();
    glfwSetWindowRefreshCallback(glfw_window, [](GLFWwindow*){
        Perekop::render(false);
    });

    double accum_fps{0}, accum_tps{0};
    while (!glfwWindowShouldClose(glfw_window)) {
        double dt = frame_timer.delta();
        double itps = 1.0 / World::tps, ifps = 1.0 / World::fps;

        accum_fps += dt; accum_tps += dt;

        if (accum_tps > itps) {
            glfwPollEvents();
            int queued_ticks = accum_tps / itps;

            if (queued_ticks > 5) {
                // drop to prevent queued ticks from exploding
                printf("Dropped %i ticks\n", queued_ticks - 1);
                accum_tps -= (queued_ticks - 1) * itps;
                queued_ticks = 1;
            }

            frame_timer.begin();
            while (accum_tps > itps) {
                step_physics(itps);
                on_step(dt);
                accum_tps -= itps;
            }
            double tick_time = frame_timer.stop();
            printf("Ran %i tick(s) in %.2fms/%.2fms (%%%2.2f UTIL)\n", 
                queued_ticks, 
                1000 * tick_time,
                1000 * itps,
                tick_time * World::tps * 100
            );
        }

        if (accum_fps > ifps) {
            accum_fps = mod(accum_fps, ifps);
            render(true);
        }

        // Time::sleep rounds to closest 1/60s for some reason...
        // this yielding meta makes it match ifps/tps perfectly
        std::this_thread::yield();
    }

    on_exit();
    glfwTerminate();
}