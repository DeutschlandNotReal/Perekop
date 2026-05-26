#pragma once
#include <pk/Scene.hpp>
#include <pk/Window.hpp>

namespace Perekop {
    extern float target_fps;
    extern pk::Window window;
    extern pk::Scene scene; 
    extern pk::Camera camera;

    extern void on_launch();
    extern void on_close();
    extern void on_step(double dt);
}