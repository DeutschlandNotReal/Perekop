#include "./internal.hpp"

#define CGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include <PK/tick.hpp>
#include <PK/Util/file.hpp>
#include <PK/Render/Mesh.hpp>
#include <PK/Render/window.hpp>
#include <PK/Util/time.hpp>

#include <stb_image.h>
#include <cgltf.h>
#include <glad/glad.h>
#include <cstdio>

using namespace pk;
using namespace Perekop;

bool begin() {
    if (!window::init()) {
        std::fputs("Failed to initialize GLFW\n", stderr);
        return false;
    }

    window::setContextVersion(4, 3);
    if (!mainWindow.create(720, 480, "Perekop")) {
        std::fputs("Failed to create GLFW window\n", stderr);
        return false;
    }

    mainWindow.makeContextCurrent();
    if (!gladLoadGLLoader((GLADloadproc)window::getProcAddress)) {
        std::fputs("Failed to load OpenGL functions\n", stderr);
        return false;
    }
    mainWindow.swapInterval(1); // VSYNC!
    mainWindow.show();
    glEnable(GL_DEPTH_TEST);

    meshVAO = VertexArr::generate();
    meshVAO.bind();
    VertexArr::Builder()
        .vec(3) // pos
        .vec(3) // nor
        .vec(2) // uv
    ;

    init::render();

    mainWindow.onRefresh.bind([]{ step::render(); });

    Perekop::onInit();

    return true;
}

int main() {
    if (!begin()) {
        mainWindow.exit();
        window::terminate();
        return 1;
    }

    time::Tracker<float, 4> timer;
    constexpr int maxSteps{4};
    float dt = 1.f / fps;

    timer.begin();
    while (!mainWindow.shouldClose()) {
        if (timer.elapsed() >= dt) {     
            window::pollEvents();
            float fdt = timer.delta();

            int ticks = pk::min((int)(fdt / dt), maxSteps);
            
            for (int i{0}; i < ticks; ++i) {
                step::physics(dt);
                Perekop::onStep(dt);
            }

            step::render();
        };

        time::sleepms(1);
    }

    Perekop::onExit(); 
    mainWindow.exit();
    window::terminate();
    return 0;
}