#include "pk/Mesh.hpp"
#include "pkutil/File.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>   

#include <pk/Engine.hpp>
#include <pkutil/Time.hpp>

using glm::vec3, glm::vec2; 
using namespace pk;

namespace pk {
    const char* v_preamble;
    const char* f_preamble;
}
namespace Perekop {
    static Mesh::Material defmat;
    Scene scene;
    Window main_window;
    Camera camera;
    float target_fps = 120.f;
}

void draw() {
    if (Perekop::main_window.visible()) {
        Perekop::scene.draw(Perekop::main_window); 
        Perekop::main_window.swap();
    }
}

int main() {
    v_preamble = File::read("pkengine/extra/pre_vsrc.glsl");
    f_preamble = File::read("pkengine/extra/pre_fsrc.glsl");
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    Perekop::main_window = Window("Perekop", 720, 480);
    Perekop::main_window.set_context();
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(0);

    Perekop::defmat = Mesh::Material(
        "void main() { gl_Position = VP * model() * vec4(_pos, 1.0); }",
        "void main() { fragColor = vec4(1.0, 0.0, 0.0, 1.0); }"
    );

    Perekop::main_window.show();
    Perekop::on_launch();
    
    StackTimer<float, 2> frame_timer;

    glEnable(GL_DEPTH_TEST);
    frame_timer.begin();
    while (!Perekop::main_window.should_close()) {
        float dt = frame_timer.delta();
        draw();
        glfwPollEvents();
        Perekop::on_step(dt);
        float fdt = frame_timer.elapsed();
        frame_timer.sleep(1.f / Perekop::target_fps - fdt);
    }

    Perekop::on_close();
    glfwTerminate();
}