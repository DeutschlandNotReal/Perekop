#define PK_INTERNAL
#include "./internal.hpp"

#define CGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include <PK/Util/file.hpp>
#include <PK/Render/mesh.hpp>
#include <PK/Render/window.hpp>
#include <PK/Util/time.hpp>

#include <stb_image.h>
#include <cgltf.h>
#include <chrono>
#include <thread>
#include <glad/glad.h>
#include <cstdio>

using namespace pk;
using namespace Perekop;

bool init() {
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

    meshVAO = vtxAttribute::generate();
    meshVAO.bind();
    vtxAttribute::Builder()
        .vec(3) // pos
        .vec(3) // nor
        .vec(2) // uv
        .setBD(1, 1)
        .mat(4) // transform
        .vec(4) // scale
    ;

    renderBegin();
    
    mainWindow.connectRefresh([] {
        Perekop::renderStep();
    });

    Perekop::onInit();

    return true;
}

void step(float dt) {
    Perekop::onStep(dt);
}

void exit() {
    Perekop::onExit();
}

int main() {
    if (!init()) {
        mainWindow.exit();
        window::terminate();
        return 1;
    }

    time::tracker<float, 4> timer;
    constexpr int maxSteps{4};
    float dt = 1.f / fps;

    timer.begin();
    while (!mainWindow.shouldClose()) {
        if (timer.elapsed() >= dt) {     
            window::pollEvents();
            float fdt = timer.delta();

            int ticks = pk::min((int)(fdt / dt), maxSteps);
            
            for (int i{0}; i < ticks; ++i) {
                Perekop::physicsStep(dt);
                step(dt);
            }

            renderStep();
        };

        time::sleepms(1);
    }

    exit(); 
    mainWindow.exit();
    window::terminate();
    return 0;
}