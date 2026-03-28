#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/vec2.hpp"
#include <stdexcept>
#include <string>
#include <functional>

// BLOAT!!
namespace pk {
    class Window;

    class Window {
        private:
            GLFWwindow* win = nullptr;
        public:
            Window(int x, int y, std::string title) {
                win = glfwCreateWindow(x, y, title.c_str(), NULL, NULL);
                if (!win) throw std::runtime_error("Failed to create window: \"" + title + "\"");
            }
            ~Window() { glfwDestroyWindow(win); }

            glm::vec2 get_size() { 
                int w, h;
                glfwGetWindowSize(win, &w, &h); 
                return glm::vec2(w, h);
            }

            glm::vec2 get_pos() {
                int x, y;
                glfwGetWindowPos(win, &x, &y);
                return glm::vec2(x, y);
            }

            std::string get_title() {
                return std::string(glfwGetWindowTitle(win));
            }

            void set_size(int w, int h) { glfwSetWindowSize(win, w, h); }
            void set_size(glm::vec2 size) { set_size(size.x, size.y); }
            void set_pos(int x, int y) { glfwSetWindowPos(win, x, y); }
            void set_pos(glm::vec2 pos) { set_pos(pos.x, pos.y); } 
            void set_title(const std::string& title) { glfwSetWindowTitle(win, title.c_str()); }
            void show() { glfwShowWindow(win); }
            void hide() { glfwHideWindow(win); }
            void maximize() { glfwMaximizeWindow(win); }
            void minimize() { glfwIconifyWindow(win); }
            void make_current() { glfwMakeContextCurrent(win); }
            void focus() { glfwFocusWindow(win); }
            void notif() { glfwRequestWindowAttention(win); }

            bool is_open() { return !glfwWindowShouldClose(win); }

            GLFWwindow* glfw() { return win; }

            Window(const Window&) = delete;
            Window& operator=(const Window&) = delete;

            std::function<void(int x, int y)> cb_resize;
            std::function<void(void)> cb_focus;
    };
}