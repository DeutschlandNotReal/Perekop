#pragma once
#include <Perekop/Event.hpp>
#include <Perekop/Model.hpp>
#include <Perekop/Mesh.hpp>
#include <Perekop/Window.hpp>

namespace Perekop {
    extern float target_fps;
    extern pk::Window main_window;

    extern pk::Mesh& create_mesh();
    extern pk::Mesh& create_mesh(pk::MeshMaterial mat);
    extern pk::Camera camera;

    extern void on_launch();
    extern void on_close();
    extern void on_step(float dt);
}