#include <PK/Connections/step.hpp>
#include <PK/Render/window.hpp>
#include <cstdio>
#include <PK/Systems/camera.hpp>
#include <PK/Scene/scene.hpp>

using namespace Perekop;
const std::filesystem::path assets = "assets";

void Perekop::OnStep(double dt) {
    PKG::StepCamera(pk::Lighting::Camera, dt);
}

void Perekop::OnRender() {

}

void Perekop::OnLaunch() {
    printf("Game begin\n");
    Window::SetIcon(assets / "images/icon.ico");
    pk::Scene::Initialize(assets);
}

void Perekop::OnExit() {
    printf("game's gone\n");
} 