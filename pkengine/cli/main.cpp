#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Perekop.hpp>
#include <Internal.hpp>
#include <pkutil/Time.hpp>
#include <pkutil/File.hpp>

using namespace pk;
using namespace glm;

static Mesh::Material default_material;

void Perekop::exit() { glfwDestroyWindow(glfw_window); }

int main() {
    Perekop::preamble_v = File::read("pkengine/extra/pre_vsrc.glsl");
    Perekop::preamble_f = File::read("pkengine/extra/pre_fsrc.glsl");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    Perekop::glfw_window = glfwCreateWindow(720, 480, "Perekop", glfwGetPrimaryMonitor(),nullptr);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(0);
    glfwShowWindow(Perekop::glfw_window);

    default_material = Mesh::Material(
        "void main() { gl_Position = P * V * model() * vec4(_pos, 1.0); }",
        "void main() { fragColor = vec4(1.0, 0.0, 0.0, 1.0); }"
    );

    Perekop::on_launch();

    glEnable(GL_DEPTH_TEST);

    StackTimer<double, 2> frame_timer;
    frame_timer.begin();
    while (!glfwWindowShouldClose(Perekop::glfw_window)) {
        double dt = frame_timer.delta();
        Perekop::draw();
        glfwPollEvents();
        Perekop::on_step(dt);
        double fdt = frame_timer.elapsed();
        frame_timer.sleep(1.0/Perekop::World::fps - fdt);
    }

    Perekop::on_exit();
    glfwTerminate();
}