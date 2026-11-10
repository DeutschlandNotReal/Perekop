#define PK_INTERNAL

#include <PK/Render/Camera.hpp>
#include <PK/Render/window.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <string>
#include <utility>

using namespace pk;

window::window() noexcept:
    input(*this)
{}

window::~window() noexcept {
    exit();
}

bool window::create(int width, int height, const char* title) noexcept {
    if (handle) return false;
    handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!handle) return false;
    glfwSetWindowUserPointer(handle, this);
    input.lastPosition = input.mousePosition();
    installCallbacks();
    return true;
}

void window::installCallbacks() noexcept {
    glfwSetWindowSizeCallback(handle, [](GLFWwindow* native, int width, int height) {
        auto* self = (window*)glfwGetWindowUserPointer(native);
        if (!self) return;

        glViewport(0, 0, width, height);
        self->onResize.invoke({width, height});
    });

    glfwSetWindowRefreshCallback(handle, [](GLFWwindow* native) {
        auto* self = static_cast<window*>(glfwGetWindowUserPointer(native));
        if (!self) return;

        self->onRefresh.invoke();
    });

    glfwSetKeyCallback(handle, [](GLFWwindow* native, int key, int, int action, int) {
        auto* self = (window*)glfwGetWindowUserPointer(native);
        if (!self) return;
        
        if (action == GLFW_PRESS)
            self->input.onKeyDown.invoke(key);
        else if (action == GLFW_RELEASE)
            self->input.onKeyUp.invoke(key);
    });

    glfwSetMouseButtonCallback(handle, [](GLFWwindow* native, int button, int action, int) {
        auto* self = (window*)glfwGetWindowUserPointer(native);
        if (!self) return;
     
        if (action == GLFW_PRESS)
            self->input.onMouseDown.invoke((mouseButton) button);
        else if (action == GLFW_RELEASE)
            self->input.onMouseUp.invoke((mouseButton) button);
    });

    glfwSetScrollCallback(handle, [](GLFWwindow* native, double, double y) {
        auto* self = (window*)glfwGetWindowUserPointer(native);

        if (!self) return;
        self->input.onScroll.invoke(y);
    });

    glfwSetCursorPosCallback(handle, [](GLFWwindow* native, double x, double y) {
         auto* self = (window*)glfwGetWindowUserPointer(native);
        if (!self) return;
        vec2 position{x, y};

        vec2 delta = position - exchange(self->input.lastPosition, position);

        if (dot(delta, delta) == 0.f) return;

        if (exchange(self->input.skipMove, false)) return;

        self->input.onMove.invoke(delta);
    });
}

bool window::init() noexcept {
    return glfwInit() == GLFW_TRUE;
}

void window::terminate() noexcept {
    glfwTerminate();
}

void window::setContextVersion(int major, int minor) noexcept {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

window::ProcAddress window::getProcAddress(const char* name) noexcept {
    return glfwGetProcAddress(name);
}

void window::pollEvents() noexcept {
    glfwPollEvents();
}

void window::setTitle(std::string_view title) noexcept {
    std::string owned{title};
    glfwSetWindowTitle(handle, owned.c_str());
}

bool window::setIcon(const std::filesystem::path& path) noexcept {
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

void window::setSize(vec2 size) noexcept {
    glfwSetWindowSize(handle, static_cast<int>(size.x), static_cast<int>(size.y));
}

vec2 window::size() const noexcept {
    int width, height;
    glfwGetWindowSize(handle, &width, &height);
    return {static_cast<float>(width), static_cast<float>(height)};
}

void window::maximize() noexcept {
    glfwMaximizeWindow(handle);
}

void window::minimize() noexcept {
    glfwIconifyWindow(handle);
}

void window::exit() noexcept {
    if (!handle) return;
    glfwDestroyWindow(handle);
    handle = nullptr;
}

void window::show() noexcept {
    glfwShowWindow(handle);
}

bool window::shouldClose() const noexcept {
    return !handle || glfwWindowShouldClose(handle);
}

void window::makeContextCurrent() noexcept {
    glfwMakeContextCurrent(handle);
}

void window::swapInterval(int interval) noexcept {
    glfwSwapInterval(interval);
}

void window::swapBuffers() noexcept {
    glfwSwapBuffers(handle);
}

void window::input::setPosition(vec2 pos) noexcept {
    lastPosition = pos;
    glfwSetCursorPos(owner->handle, pos.x, pos.y);
}

void window::input::lockMouse() noexcept {
    if (mouseLocked()) return;
    skipMove = true;
    glfwSetInputMode(owner->handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void window::input::unlockMouse() noexcept {
    skipMove = false;
    glfwSetInputMode(owner->handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool window::input::mouseLocked() const noexcept {
    return glfwGetInputMode(owner->handle, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

vec2 window::input::mousePosition() const noexcept {
    double x, y;
    glfwGetCursorPos(owner->handle, &x, &y);
    return {x, y};
}

pose window::input::mousePose() const noexcept {
    if (!owner->mainCamera) return {};
    vec2 windowSize = owner->size();
    if (windowSize.x == 0.f || windowSize.y == 0.f) return {};

    vec2 uv = mousePosition() / windowSize * 2.f - 1.f;
    float fov = owner->mainCamera->tanFov();
    vec3 local = normalize(vec3{uv.x * fov * windowSize.x / windowSize.y, -uv.y * fov, -1.f});
    return pose::lookAt(owner->mainCamera->pose.pos,
        worldspace(local, owner->mainCamera->pose), owner->mainCamera->pose.up());
}

bool window::input::mouseHeld(mouseButton button) const noexcept {
    return glfwGetMouseButton(owner->handle, (int)button) == GLFW_PRESS;
}

bool window::input::keyHeld(int key) const noexcept {
    return glfwGetKey(owner->handle, key) == GLFW_PRESS;
}