#define PK_INTERNAL

#include <PK/Render/camera.hpp>
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
        auto* self = static_cast<window*>(glfwGetWindowUserPointer(native));
        if (!self) return;
        glViewport(0, 0, width, height);
        invokeListeners(self->resizeListeners, vec2{static_cast<float>(width), static_cast<float>(height)});
    });

    glfwSetWindowRefreshCallback(handle, [](GLFWwindow* native) {
        auto* self = static_cast<window*>(glfwGetWindowUserPointer(native));
        if (!self) return;
        invokeListeners(self->refreshListeners);
    });

    glfwSetKeyCallback(handle, [](GLFWwindow* native, int key, int, int action, int) {
        auto* self = static_cast<window*>(glfwGetWindowUserPointer(native));
        if (!self) return;
        if (action == GLFW_PRESS)
            invokeListeners(self->input.keyPressListeners, key);
        else if (action == GLFW_RELEASE)
            invokeListeners(self->input.keyReleaseListeners, key);
    });

    glfwSetMouseButtonCallback(handle, [](GLFWwindow* native, int button, int action, int) {
        auto* self = static_cast<window*>(glfwGetWindowUserPointer(native));
        if (!self) return;
     
        if (action == GLFW_PRESS)
            invokeListeners(self->input.mousePressListeners, (mouseButton)button);
        else if (action == GLFW_RELEASE)
            invokeListeners(self->input.mouseReleaseListeners, (mouseButton)button);
    });

    glfwSetScrollCallback(handle, [](GLFWwindow* native, double, double y) {
        auto* self = static_cast<window*>(glfwGetWindowUserPointer(native));
        if (!self) return;
        invokeListeners(self->input.mouseScrollListeners, static_cast<float>(y));
    });

    glfwSetCursorPosCallback(handle, [](GLFWwindow* native, double x, double y) {
        auto* self = static_cast<window*>(glfwGetWindowUserPointer(native));
        if (!self) return;
        vec2 position{static_cast<float>(x), static_cast<float>(y)};
        vec2 delta = position - std::exchange(self->input.lastPosition, position);
        if (dot(delta, delta) == 0.f) return;
        if (std::exchange(self->input.skipMove, false)) return;
        invokeListeners(self->input.mouseMoveListeners, delta);
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

void window::connectRefresh(consumer<> listener) noexcept {
    refreshListeners.push(std::move(listener));
}

void window::connectResize(consumer<vec2> listener) noexcept {
    resizeListeners.push(std::move(listener));
}

void window::setTitle(std::string_view title) noexcept {
    std::string owned{title};
    glfwSetWindowTitle(native(), owned.c_str());
}

bool window::setIcon(const std::filesystem::path& path) noexcept {
    if (!handle) return false;

    int width, height;
    std::string filename = path.string();
    unsigned char* pixels = stbi_load(filename.c_str(), &width, &height, nullptr, 4);
    if (!pixels) return false;

    GLFWimage image{width, height, pixels};
    glfwSetWindowIcon(native(), 1, &image);
    stbi_image_free(pixels);
    return true;
}

void window::setSize(vec2 size) noexcept {
    glfwSetWindowSize(native(), static_cast<int>(size.x), static_cast<int>(size.y));
}

vec2 window::size() const noexcept {
    int width, height;
    glfwGetWindowSize(native(), &width, &height);
    return {static_cast<float>(width), static_cast<float>(height)};
}

void window::maximize() noexcept {
    glfwMaximizeWindow(native());
}

void window::minimize() noexcept {
    glfwIconifyWindow(native());
}

void window::exit() noexcept {
    if (!handle) return;
    glfwDestroyWindow(handle);
    handle = nullptr;
}

void window::show() noexcept {
    glfwShowWindow(native());
}

bool window::shouldClose() const noexcept {
    return !handle || glfwWindowShouldClose(native());
}

void window::makeContextCurrent() noexcept {
    glfwMakeContextCurrent(native());
}

void window::swapInterval(int interval) noexcept {
    glfwSwapInterval(interval);
}

void window::swapBuffers() noexcept {
    glfwSwapBuffers(native());
}

void window::input::bindMove(consumer<vec2> listener) noexcept {
    mouseMoveListeners.push(std::move(listener));
}

void window::input::bindScroll(consumer<float> listener) noexcept {
    mouseScrollListeners.push(std::move(listener));
}

void window::input::bindMPress(consumer<mouseButton> listener) noexcept {
    mousePressListeners.push(std::move(listener));
}

void window::input::bindMRelease(consumer<mouseButton> listener) noexcept {
    mouseReleaseListeners.push(std::move(listener));
}

void window::input::bindKPress(consumer<int> listener) noexcept {
    keyPressListeners.push(std::move(listener));
}

void window::input::bindKRelease(consumer<int> listener) noexcept {
    keyReleaseListeners.push(std::move(listener));
}

void window::input::setPosition(vec2 position) noexcept {
    lastPosition = position;
    glfwSetCursorPos(owner->native(), static_cast<double>(position.x), static_cast<double>(position.y));
}

void window::input::lockMouse() noexcept {
    if (mouseLocked()) return;
    skipMove = true;
    glfwSetInputMode(owner->native(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void window::input::unlockMouse() noexcept {
    skipMove = false;
    glfwSetInputMode(owner->native(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool window::input::mouseLocked() const noexcept {
    return glfwGetInputMode(owner->native(), GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

vec2 window::input::mousePosition() const noexcept {
    double x, y;
    glfwGetCursorPos(owner->native(), &x, &y);
    return {static_cast<float>(x), static_cast<float>(y)};
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
    return glfwGetMouseButton(owner->native(), static_cast<int>(button)) == GLFW_PRESS;
}

bool window::input::keyHeld(int key) const noexcept {
    return glfwGetKey(owner->native(), key) == GLFW_PRESS;
}