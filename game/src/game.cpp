#include <PK/step.hpp>
#include <PK/file.hpp>
#include <PK/window.hpp>
#include <PK/shader.hpp>
#include <glm/gtc/random.hpp>

#include <cstdio>
#include <PKG/camera.hpp>
using namespace pk;
using namespace Perekop;

const path assets = path("game") / "assets";

namespace RenderState {
    vector<Mesh> meshes;
    vector<Model> models;

    ShaderProgram shader;
    Texture texture;

    Camera camera;
    mat4 VP;
    Renderer renderer;
}

void Perekop::OnStep(double dt) {
    PKG::StepCamera(RenderState::camera, dt);
}

void Perekop::OnRender() {
    using namespace RenderState;
    VP = camera.proj() * camera.view();

    renderer.fill({0.2, 0.2, 0.2});
    renderer.clear(depthBuffer | colourBuffer);
    renderer.ready_models(models);

    if (meshes.size() > 0) {
        renderer.draw(shader, meshes.back());
    }

    renderer.swap();
}

void Perekop::OnLaunch() {
    printf("Game begin\n");
    Window::SetIcon(assets / "images/icon.ico");
    RenderState::camera.pose = inverse(lookAt(vec3{25, 25, 25}, vec3{0, 0, 0}, vec3{0, 1, 0}));
    PKG::InitCamera(RenderState::camera);

    RenderState::meshes.push(
        assets / "models/Untitled.glb",
        assets / "models/PK67.glb"
    );

    for (Mesh& mesh : RenderState::meshes) mesh.load();

    RenderState::shader = ShaderProgram(
        Shader(Shader::vertex, assets / "shaders/vert.glsl"),
        Shader(Shader::fragment, assets / "shaders/frag.glsl"),
        {
            {.name = "camera", .type = ShaderProgram::u_mat4, .data = &RenderState::VP},
            {.name = "T", .type = ShaderProgram::texture, .data = &RenderState::texture}
        } 
    );

    RenderState::texture = assets / "images/test.jpg";

    for (int x = 0; x < 15; x++) {
        for (int y = 0; y < 15; y++) {
            for (int z = 0; z < 15; z++) {
                RenderState::models.emplace(
                    ballRand(25.f),
                    vec3{1, 1, 1}, 
                    vec4{x, y, z, x}
                );
            }
        }
    }
}

void Perekop::OnExit() {
    printf("game's gone\n");
} 