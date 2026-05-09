#include "Perekop/Mesh.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>   

#include <Perekop/Engine.hpp>
#include <Perekop/Time.hpp>

using glm::vec3, glm::vec2; 
using namespace pk;

namespace Perekop {
    static MeshMaterial defmat = MeshMaterial();
    static MeshRenderer renderer = MeshRenderer();
    float target_fps = 60.f;

    Window main_window("Perekop", 720, 480);
    Mesh& create_mesh() { return renderer.create(defmat); }
    Mesh& create_mesh(MeshMaterial mat) { return renderer.create(mat); }

    Camera camera{};
}

void draw() {
    glm::vec2 size = Perekop::main_window.size();

    if (size.x + size.y > 0) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Perekop::renderer.draw(); 
        Perekop::main_window.swap_buffers();
    }

    glfwPollEvents();
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Perekop::main_window.make_context();

    Perekop::defmat = MeshMaterial(
        "void main() { gl_Position = VP * model() * vec4(_pos, 1.0); }",
        "void main() { fragColor = vec4(1.0, 0.0, 0.0, 1.0); }"
    );

    Perekop::main_window.show();
    Perekop::on_launch();
    
    StackTimer<float, 2> frame_timer;

    frame_timer.begin(-1.f/Perekop::target_fps);
    while (!Perekop::main_window.should_close()) {
        draw();
        float dt = frame_timer.delta();
        Perekop::on_step(dt);
        frame_timer.sleep(1/Perekop::target_fps - dt);
    }

    Perekop::on_close();
    glfwTerminate();
}