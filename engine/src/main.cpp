#define PK_INTERNAL
#define GLFW_EXPOSE_NATIVE_WIN32
#define CGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <cgltf.h>

#include <windows.h>
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <PKINT/internal.hpp>
#include <PK/time.hpp>
#include <PK/file.hpp>
#include <PK/window.hpp>

using namespace pk;
using namespace Perekop;

void Perekop::exit() { glfwDestroyWindow(glfw_window); }

void Perekop::Window::icon(const path& path) noexcept {
    HWND hwnd = glfwGetWin32Window(glfw_window);

    HICON icon = (HICON)LoadImageW(
        nullptr,
        path.c_str(),
        IMAGE_ICON,
        0, 0,
        LR_LOADFROMFILE | LR_DEFAULTSIZE
    );

    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
}

void init() {
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
    on_launch();
    
    glfwSetWindowRefreshCallback(glfw_window, [](GLFWwindow*){
        Perekop::on_render();
    });
}

int main() {
    init();
    time::Tracker<double, 2> frame_timer;

    double accumulator{0};
    double rfps = 1.0 / World::fps;
    frame_timer.begin();
    while (!glfwWindowShouldClose(glfw_window)) {
        accumulator += frame_timer.delta();

        if (accumulator >= rfps) {
            glfwPollEvents();
            int ticks = accumulator * World::fps;
            accumulator -= ticks * rfps;
            
            ticks = std::min(ticks, 4);

            while (ticks-- > 0) {
                step_physics(rfps);
                on_step(rfps);
            } 

            Perekop::on_render();
        }

        std::this_thread::yield();
    }
    on_exit(); 

    glfwTerminate();
}