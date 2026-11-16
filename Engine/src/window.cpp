#define PK_INTERNAL

#include <PK/Render/Camera.hpp>
#include <PK/Render/window.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <string>
#include <utility>

using namespace pk;

Window::~Window() noexcept { exit(); }

bool Window::create(int width, int height, const char* title) noexcept {
    if (handle) return false;
    handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!handle) return false;
    glfwSetWindowUserPointer(handle, this);
    lmpos = mousePos();
    
     glfwSetWindowSizeCallback(handle, [](GLFWwindow* native, int width, int height) {
        auto* self = (Window*)glfwGetWindowUserPointer(native);
        if (!self) return;

        glViewport(0, 0, width, height);
        self->onResize.invoke({width, height});
    });

    glfwSetWindowRefreshCallback(handle, [](GLFWwindow* native) {
        auto* self = (Window*)glfwGetWindowUserPointer(native);
        if (!self) return;

        self->onRefresh.invoke();
    });

    glfwSetKeyCallback(handle, [](GLFWwindow* native, int key, int, int action, int) {
        auto* self = (Window*)glfwGetWindowUserPointer(native);
        if (!self) return;
        
        if (action == GLFW_PRESS)
            self->onKeyDown.invoke(key);
        else if (action == GLFW_RELEASE)
            self->onKeyUp.invoke(key);
    });

    glfwSetMouseButtonCallback(handle, [](GLFWwindow* native, int button, int action, int) {
        auto* self = (Window*)glfwGetWindowUserPointer(native);
        if (!self) return;
     
        if (action == GLFW_PRESS)
            self->onMouseDown.invoke((MouseKey) button);
        else if (action == GLFW_RELEASE)
            self->onMouseUp.invoke((MouseKey) button);
    });

    glfwSetScrollCallback(handle, [](GLFWwindow* native, double, double y) {
        auto* self = (Window*)glfwGetWindowUserPointer(native);

        if (!self) return;
        self->onMouseScroll.invoke(y);
    });

    glfwSetCursorPosCallback(handle, [](GLFWwindow* native, double x, double y) {
        auto* self = (Window*)glfwGetWindowUserPointer(native);
        if (!self) return;
        vec2 position{x, y};

        vec2 delta = position - exchange(self->lmpos, position);

        if (dot(delta, delta) == 0.f) return;

        if (exchange(self->nomove, false)) return;

        self->onMouseMove.invoke(delta);
    });

    return true;
}

void Window::setTitle(std::string_view title) noexcept {
    std::string owned{title};
    glfwSetWindowTitle(handle, owned.c_str());
}

bool Window::setIcon(const std::filesystem::path& path) noexcept {
    if (!handle) return false;

    int width, height;
    std::string filename = path.string();
    unsigned char* pixels = stbi_load(filename.c_str(), &width, &height, nullptr, 4);
    if (!pixels) return false;

    GLFWimage image{width, height, pixels};
    glfwSetWindowIcon(handle, 1, &image);
    stbi_image_free(pixels);
    return true;
}

void Window::setSize(vec2 size) noexcept {
    glfwSetWindowSize(handle, static_cast<int>(size.x), static_cast<int>(size.y));
}

vec2 Window::size() const noexcept {
    int width, height;
    glfwGetWindowSize(handle, &width, &height);
    return {static_cast<float>(width), static_cast<float>(height)};
}

void Window::maximize() noexcept {
    glfwMaximizeWindow(handle);
}

void Window::minimize() noexcept {
    glfwIconifyWindow(handle);
}

void Window::exit() noexcept {
    if (!handle) return;
    glfwDestroyWindow(handle);
    handle = nullptr;
}

void Window::show() noexcept {
    glfwShowWindow(handle);
}

bool Window::closing() const noexcept {
    return !handle || glfwWindowShouldClose(handle);
}

void Window::makeContextCurrent() noexcept {
    glfwMakeContextCurrent(handle);
}

void Window::mousePos(vec2 pos) noexcept {
    lmpos = pos;
    glfwSetCursorPos(*this, pos.x, pos.y);
}

void Window::setMouseLocked(bool locked) noexcept {
    nomove = locked;
    glfwSetInputMode(*this, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

bool Window::isMouseLocked() const noexcept {
    return glfwGetInputMode(*this, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

vec2 Window::mousePos() const noexcept {
    double x, y;
    glfwGetCursorPos(*this, &x, &y);
    return {x, y};
}

pose Window::mousePose() const noexcept {
    vec2 windowSize = size();
    if (windowSize.x == 0.f || windowSize.y == 0.f) return {};

    vec2 uv = mousePos() / windowSize * 2.f - 1.f;
    float fov = camera->tanFov();
    vec3 local = normalize(vec3{uv.x * fov * windowSize.x / windowSize.y, -uv.y * fov, -1.f});
    return pose::lookAt(camera->pose.pos,
        worldspace(local, camera->pose), camera->pose.up());
}

bool Window::held(MouseKey button) const noexcept {
    return glfwGetMouseButton(*this, (int)button) == GLFW_PRESS;
}

bool Window::held(int key) const noexcept {
    return glfwGetKey(*this, key) == GLFW_PRESS;
}