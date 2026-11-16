#include "./internal.hpp"

#define CGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include <PK/tick.hpp>
#include <PK/world.hpp>
#include <PK/Util/file.hpp>
#include <PK/Render/Mesh.hpp>
#include <PK/Render/window.hpp>
#include <PK/Util/time.hpp>

#include <stb_image.h>
#include <cgltf.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>

using namespace pk;
using namespace Perekop;

bool begin() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Window::main.create(720, 480, "Perekop");
    glfwMakeContextCurrent(Window::main);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    Window::main.show();
    glEnable(GL_DEPTH_TEST);

    meshVAO = VertexArr::generate();
    meshVAO.bind();
    VertexArr::Builder()
        .vec(3) // pos
        .vec(3) // nor
        .vec(2) // uv
    ;

    init::render();
    init::drawer();

    Window::main.onRefresh.bind([]{ step::drawer(0); });

    Perekop::onInit();

    return true;
}

int main() {
    begin();

    time::Tracker<float, 4> timer;
    constexpr int maxSteps{4};
    float dt = 1.f / fps;

    timer.begin();
    while (!Window::main.closing()) {
        if (timer.elapsed() >= dt) {     
            glfwPollEvents();
            float fdt = timer.delta();

            int ticks = pk::min((int)(fdt / dt), maxSteps);
            
            for (int i{0}; i < ticks; ++i) {
                step::physics(dt);
                Perekop::onStep(dt);
            }

            Perekop::ModelBVH.construct({
                ModelHandle::items.begin(), 
                ModelHandle::items.end()
            });

            step::drawer(fdt);
        };

        time::sleepms(1);
    }

    Perekop::onExit(); 
    Window::main.exit();
    glfwTerminate();
    return 0;
}