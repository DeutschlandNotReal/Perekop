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

void Perekop::ExitGame() { glfwDestroyWindow(glfw_window); }

void Perekop::Window::SetIcon(const path& path) noexcept {
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

    RenderBegin();
    WindowBegin();
    OnLaunch();
    
    glfwSetWindowRefreshCallback(glfw_window, [](GLFWwindow*){
        Perekop::OnRender();
    });
}

int main() {
    init();
    time::Tracker<double, 2> FrameTimer;

    double accumulator{0};

    double frameperiod = 1.0 / fps;
    FrameTimer.Begin();

    while (!glfwWindowShouldClose(glfw_window)) {
        accumulator += FrameTimer.Delta();

        if (accumulator >= frameperiod) {
            frameperiod = 1.0 / fps;
            glfwPollEvents();
            int ticks = accumulator * fps;
            accumulator -= ticks * frameperiod;
            
            ticks = std::min(ticks, 4);

            while (ticks-- > 0) {
                OnStep(frameperiod);
            } 
            
            OnRender();
        }
        std::this_thread::yield();
    }
    OnExit(); 

    glfwTerminate();
}