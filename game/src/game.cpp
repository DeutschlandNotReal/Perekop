#include <PK/step.hpp>
#include <PK/file.hpp>
#include <PK/window.hpp>
#include <PK/shader.hpp>
#include <PK/userinput.hpp>
#include <PK/collision.hpp>
#include <glm/gtc/random.hpp>

#include <cstdio>
#include <PKG/camera.hpp>
#include <PK/time.hpp>
using namespace pk;
using namespace Perekop;
const path assets = path("game") / "assets";

namespace RenderState {
    vector<Mesh> meshes;
    vector<Model> models;
    vector<vec3> model_scales;

    Shader shader;
    Shader shadow_shader;
    Texture texture;

    Camera camera;
    Camera light;

    mat4 VP;
    mat4 lightVP;
    Render renderer;

    Texture shadow;
    Framebuffer shadow_buffer;
}

float light_phase{0.f};

void Perekop::OnStep(double dt) {
    PKG::StepCamera(RenderState::camera, dt);

    light_phase += dt * .9f;
    float orbit = 60.f;
    RenderState::light.pose = pose::LookAt(
        {sin(light_phase) * orbit, 42.f + cos(light_phase * 1.3f) * 12.f, cos(light_phase) * orbit},
        {0, 3.0f, 0},
        {0, 1, 0}
    );
}

time::Tracker<double, 3> RenderTracker;
void Perekop::OnRender() {
    using namespace RenderState;

    RenderTracker.Begin();
    VP = camera.Projection() * camera.View();
    lightVP = light.Projection() * light.View();

    renderer.Target(shadow_buffer, Render::None);
    renderer.Fill({1.f, 1.f, 1.f});
    renderer.Clear(DepthBuffer);
    if (meshes.size() > 0) {
        renderer.Draw(shadow_shader, meshes.back(), models);
    }

    renderer.Viewport();
    renderer.Fill({0.2, 0.2, 0.2});
    renderer.Clear(DepthBuffer | ColorBuffer);
    if (meshes.size() > 0) {
        renderer.Draw(shader, meshes.back(), models);
    }
    renderer.Swap();
    double render_T = RenderTracker.Stop();

    printf("Render Time %.3fs\n", render_T);
}  

void Perekop::OnLaunch() {
    printf("Game begin\n");
    Window::SetIcon(assets / "images/icon.ico");
    RenderState::camera.pose = pose::LookAt({52, 20, 52}, {0, 4, 0});
    RenderState::light.pose = pose::LookAt({50, 36, 22}, {0, 2, 0});
    PKG::InitCamera(RenderState::camera);

    RenderState::shadow = Texture::Depth(2048, 2048);
    RenderState::shadow_buffer = Framebuffer(2048, 2048);
    RenderState::shadow_buffer.AttachDepth(RenderState::shadow);

    RenderState::meshes.push(
        assets / "models/Untitled.glb",
        assets / "models/PK67.glb"
    );

    for (Mesh& mesh : RenderState::meshes) mesh.Load();

    RenderState::shadow_shader = Shader({
        ShaderStage(ShaderStage::Vertex, assets / "shaders/vert.glsl"),
        ShaderStage(ShaderStage::Fragment, std::string_view(R"(
            #version 430
            out vec4 fragColor;
            void main() { fragColor = vec4(1.0); }
        )"))},
        {
            {.name = "camera", .type = Shader::Mat4, .data = &RenderState::lightVP},
            {.name = "light", .type = Shader::Mat4, .data = &RenderState::lightVP},
            {.name = "lightPos", .type = Shader::Vec3, .data = &RenderState::light.pose.pos}
        }
    );

    RenderState::shader = Shader({
        ShaderStage(ShaderStage::Vertex, assets / "shaders/vert.glsl"),
        ShaderStage(ShaderStage::Fragment, assets / "shaders/frag.glsl")},
        {
            {.name = "camera", .type = Shader::Mat4, .data = &RenderState::VP},
            {.name = "light", .type = Shader::Mat4, .data = &RenderState::lightVP},
            {.name = "lightPos", .type = Shader::Vec3, .data = &RenderState::light.pose.pos},
            {.name = "T", .type = Shader::Texture, .data = &RenderState::texture},
            {.name = "shadow", .type = Shader::Texture, .data = &RenderState::shadow}
        }
    );

    RenderState::texture = assets / "images/test.jpg";
    RenderState::shadow = Texture::Depth(2048, 2048);
    RenderState::shadow_buffer = Framebuffer(2048, 2048);
    RenderState::shadow_buffer.AttachDepth(RenderState::shadow);

    RenderState::models.clear();
    RenderState::models.emplace(pose{vec3{0, -3.5f, 0}}, vec3{60.0f, 1.0f, 60.0f}, vec4{0.08f, 0.08f, 0.08f, 0.0f});

    unsigned seed = 1337;
    auto rnd = [&seed]() noexcept {
        seed = seed * 1664525u + 1013904223u;
        return float(seed >> 8) / float(0x00ffffffu);
    };

    for (int i = 0; i < 18; ++i) {
        float x = (rnd() - .5f) * 46.f;
        float z = (rnd() - .5f) * 46.f;
        float s = 1.6f + rnd() * 5.2f;
        float h = 0.8f + rnd() * 2.8f;
        vec3 scale{s, s * h, s};
        vec3 pos{x, scale.y * 0.5f - 2.8f, z};
        vec4 color{rnd(), rnd() * .8f + .2f, .5f + rnd() * .5f, 0.f};
        RenderState::models.emplace(pose{pos}, scale, color);
    }

    for (int i = 0; i < 7; ++i) {
        float angle = float(i) / 7.f * 6.283185307f;
        float radius = 12.f + rnd() * 8.f;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        float s = 2.2f + rnd() * 3.0f;
        vec3 scale{s, s * (1.4f + rnd() * 2.0f), s};
        vec3 pos{x, scale.y * 0.5f - 2.8f, z};
        vec4 color{.35f + rnd() * .55f, .3f + rnd() * .55f, .7f + rnd() * .25f, 0.f};
        RenderState::models.emplace(pose{pos}, scale, color);
    }

    RenderState::model_scales.clear();
    for (const Model& model : RenderState::models) RenderState::model_scales.push(model.scale);
}

void Perekop::OnExit() {
    printf("game's gone\n");
} 