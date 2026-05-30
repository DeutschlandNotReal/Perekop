#include "pk/GUI.hpp"
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memoryapi.h>
#include <thread>

#include <Perekop.hpp>
#include <Internal.hpp>
#include <pkutil/Time.hpp>
#include <pkutil/File.hpp>
using namespace pk;
using namespace glm;

static Mesh::Appearance default_material;
static int f_main{0};
static bool alive{true};

void Perekop::exit() { glfwDestroyWindow(glfw_window); }

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    Perekop::glfw_window = glfwCreateWindow(720, 480, "Perekop", nullptr, nullptr);
    glfwMakeContextCurrent(Perekop::glfw_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1.0f/Perekop::World::fps);
    glfwShowWindow(Perekop::glfw_window);
    glEnable(GL_DEPTH_TEST);

    Perekop::init::draw();
    Perekop::init::window();

    default_material = Mesh::Appearance(
        "pkengine/assets/shaders/def_vsrc.glsl",
        "pkengine/assets/shaders/def_fsrc.glsl"
    );

    Perekop::on_launch();

    Timer<double, 1> mtimer;
    glfwMakeContextCurrent(nullptr);
    std::thread render_thread([](){
        glfwMakeContextCurrent(Perekop::glfw_window);
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);  
        int f{0};
        while (alive) {
            if (Perekop::window_resized) {
                int w, h;
                glfwGetWindowSize(Perekop::glfw_window, &w, &h);
                glViewport(0, 0, w, h);
                Perekop::window_resized = false;
            }
            if (f != f_main) { f = f_main; Perekop::step::draw(); }

            Timer<short,0>::sleep(0.9/Perekop::World::fps);
        }
    });

    mtimer.begin();
    while (!glfwWindowShouldClose(Perekop::glfw_window)) {
        f_main++;
        double dt = mtimer.delta();
        glfwPollEvents();
        Perekop::step::window();
        Perekop::on_step(dt);
        double fdt = mtimer.elapsed();
        mtimer.sleep(1.0/Perekop::World::fps - fdt);
    } 
    alive = false;
    render_thread.join();

    Perekop::on_exit();
    glfwTerminate();
}