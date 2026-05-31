#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memoryapi.h>

#include <Internal.hpp>
#include <pkutil/Time.hpp>
#include <pkutil/File.hpp>
using namespace pk;
using namespace glm;

static Mesh::Appearance default_material;

void Perekop::exit() { glfwDestroyWindow(glfw_window); }

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    Perekop::glfw_window = glfwCreateWindow(720, 480, "Perekop", nullptr, nullptr);
    glfwMakeContextCurrent(Perekop::glfw_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(0);
    glfwShowWindow(Perekop::glfw_window);
    glEnable(GL_DEPTH_TEST);

    Perekop::init::draw();
    Perekop::init::window();

    default_material = Mesh::Appearance(
        "pkengine/assets/shaders/def_vsrc.glsl",
        "pkengine/assets/shaders/def_fsrc.glsl"
    );

    Perekop::on_launch();

    Timer<double, 1> ftimer; ftimer.begin();
    while (!glfwWindowShouldClose(Perekop::glfw_window)) {
        double dt = ftimer.delta();
        glfwPollEvents();
        Perekop::step::draw();
        Perekop::step::window();
        Perekop::on_step(dt);
        double fdt = ftimer.elapsed();
        ftimer.sleep(1.0/Perekop::World::fps - fdt);
    } 
    Perekop::on_exit();
    glfwTerminate();
}