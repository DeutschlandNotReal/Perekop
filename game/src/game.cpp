#include <PK/step.hpp>
#include <PK/file.hpp>
#include <PK/window.hpp>
#include <PK/shader.hpp>
#include <PK/userinput.hpp>
#include <PK/collision.hpp>
#include <glm/gtc/random.hpp>

#include <cstdio>
#include <limits>
#include <PKG/camera.hpp>
#include <PK/time.hpp>
using namespace pk;
using namespace Perekop;
const path assets = path("game") / "assets";

namespace RenderState {
    vector<Mesh> meshes;
    vector<Model> models;
    vector<vec3> model_scales;

    ShaderProgram shader;
    ShaderProgram shadow_shader;
    Texture texture;

    Camera camera;
    Camera light;

    mat4 VP;
    mat4 lightVP;
    Renderer renderer;

    Texture shadow;
    Framebuffer shadow_buffer;
}

float light_phase{0.f};

void Perekop::OnStep(double dt) {
    PKG::StepCamera(RenderState::camera, dt);

    light_phase += dt * .9f;
    float orbit = 60.f;
    RenderState::light.pose = pose::lookAt(
        {sin(light_phase) * orbit, 42.f + cos(light_phase * 1.3f) * 12.f, cos(light_phase) * orbit},
        {0, 3.0f, 0},
        {0, 1, 0}
    );
}

time::Tracker<double, 3> RenderTracker;
void Perekop::OnRender() {
    using namespace RenderState;

    ray MouseRay = {Mouse::GetPose(camera)};

    /*
    float closest = std::numeric_limits<float>::infinity();
    int hover = -1;
    for (size_t i = 0; i < models.size(); ++i) {
        const Model& model = models[i];

        float dst = MouseRay.intersectBall();

        float distance = pk::distance(MouseRay, hitbox, models[i].pose);
        if (distance < closest) {
            closest = distance;
            hover = i;
        }
    }

    for (size_t i = 0; i < models.size(); ++i) {
        if (static_cast<int>(i) == hover) models[i].scale = model_scales[i] * 1.5f;
        else models[ i].scale = model_scales[i];
    }
        */

    RenderTracker.begin();
    renderer.ready_models(models);
    double ready_T = RenderTracker.stop();

    RenderTracker.begin();
    VP = camera.proj() * camera.view();
    lightVP = light.proj() * light.view();

    renderer.target(shadow_buffer, Renderer::none);
    renderer.viewport(2048, 2048);
    renderer.fill({1.f, 1.f, 1.f});
    renderer.clear(depthBuffer);
    if (meshes.size() > 0) {
        renderer.draw(shadow_shader, meshes.back());
    }

    renderer.viewport();
    renderer.fill({0.2, 0.2, 0.2});
    renderer.clear(depthBuffer | colourBuffer);
    double misc_T = RenderTracker.stop();

    RenderTracker.begin();
    if (meshes.size() > 0) {
        renderer.draw(shader, meshes.back());
    }
    renderer.swap();
    double draw_T = RenderTracker.stop();
    double t = RenderTracker.stop();

    printf("Render Time %.3fs, READY: %.3fs, MISC: %.3fs, DRAW: %.3fs\n", t, ready_T, misc_T, draw_T);
}  

void Perekop::OnLaunch() {
    printf("Game begin\n");
    Window::SetIcon(assets / "images/icon.ico");
    RenderState::camera.pose = pose::lookAt({52, 20, 52}, {0, 4, 0});
    RenderState::light.pose = pose::lookAt({50, 36, 22}, {0, 2, 0});
    PKG::InitCamera(RenderState::camera);

    RenderState::shadow = Texture::depth(2048, 2048);
    RenderState::shadow_buffer = Framebuffer(2048, 2048);
    RenderState::shadow_buffer.attach_depth(RenderState::shadow);

    RenderState::meshes.push(
        assets / "models/Untitled.glb",
        assets / "models/PK67.glb"
    );

    for (Mesh& mesh : RenderState::meshes) mesh.load();

    Shader vshader{Shader::vertex, assets / "shaders/vert.glsl"};
    RenderState::shadow_shader = ShaderProgram(
        vshader,
        Shader(Shader::fragment, std::string_view(R"(
            #version 430
            out vec4 fragColor;
            void main() { fragColor = vec4(1.0); }
        )")),
        {
            {.name = "camera", .type = ShaderProgram::u_mat4, .data = &RenderState::lightVP},
            {.name = "light", .type = ShaderProgram::u_mat4, .data = &RenderState::lightVP},
            {.name = "lightPos", .type = ShaderProgram::u_vec3, .data = &RenderState::light.pose.pos}
        }
    );

    RenderState::shader = ShaderProgram(
        vshader,
        Shader(Shader::fragment, assets / "shaders/frag.glsl"),
        {
            {.name = "camera", .type = ShaderProgram::u_mat4, .data = &RenderState::VP},
            {.name = "light", .type = ShaderProgram::u_mat4, .data = &RenderState::lightVP},
            {.name = "lightPos", .type = ShaderProgram::u_vec3, .data = &RenderState::light.pose.pos},
            {.name = "T", .type = ShaderProgram::texture, .data = &RenderState::texture},
            {.name = "shadow", .type = ShaderProgram::texture, .data = &RenderState::shadow}
        }
    );

    RenderState::texture = assets / "images/test.jpg";
    RenderState::shadow = Texture::depth(2048, 2048);
    RenderState::shadow_buffer = Framebuffer(2048, 2048);
    RenderState::shadow_buffer.attach_depth(RenderState::shadow);

    RenderState::models.clear();
    RenderState::models.emplace(pose{vec3{0, -3.5f, 0}}, vec3{60.0f, 1.0f, 60.0f}, vec4{0.08f, 0.08f, 0.08f, 0.0f});

    std::uint32_t seed = 1337u;
    auto rnd = [&seed]() noexcept {
        seed = seed * 1664525u + 1013904223u;
        return float(seed >> 8) / float(0x00ffffffu);
    };

    for (int i = 0; i < 18; ++i) {
        float x = (rnd() - 0.5f) * 46.0f;
        float z = (rnd() - 0.5f) * 46.0f;
        float s = 1.6f + rnd() * 5.2f;
        float h = 0.8f + rnd() * 2.8f;
        vec3 scale{s, s * h, s};
        vec3 pos{x, scale.y * 0.5f - 2.8f, z};
        vec4 color{rnd(), rnd() * 0.8f + 0.2f, 0.5f + rnd() * 0.5f, 0.0f};
        RenderState::models.emplace(pose{pos}, scale, color);
    }

    for (int i = 0; i < 7; ++i) {
        float angle = float(i) / 7.0f * 6.283185307f;
        float radius = 12.0f + rnd() * 8.0f;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        float s = 2.2f + rnd() * 3.0f;
        vec3 scale{s, s * (1.4f + rnd() * 2.0f), s};
        vec3 pos{x, scale.y * 0.5f - 2.8f, z};
        vec4 color{0.35f + rnd() * 0.55f, 0.30f + rnd() * 0.55f, 0.70f + rnd() * 0.25f, 0.0f};
        RenderState::models.emplace(pose{pos}, scale, color);
    }

    RenderState::model_scales.clear();
    for (const Model& model : RenderState::models) RenderState::model_scales.push(model.scale);
}

void Perekop::OnExit() {
    printf("game's gone\n");
} 