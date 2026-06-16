#include "PKLib/Geometry.hpp"

#include <Perekop.hpp>
#include <cstdio>
#include <PKLib/File.hpp>
#include <game/camera.hpp>
#include <game/gui.hpp>

using namespace pk;
using namespace glm;
using namespace Perekop;
static float t = 0;

struct body { uint16 modelid; vec3 vel, pos; float Z; };

static vector<body> bodies;
static uint fake_randomler = 0;




uint random_u32() {
    fake_randomler ^= 0xF123123FA;
    fake_randomler += 0xF2100041F;
    fake_randomler *= 0xA004112F;
    fake_randomler ^= 0x0231231F;
    return fake_randomler;
}

float random(float min = 0, float max = 1) {
    return min + (max - min) * (random_u32() / (float)0xFFFFFFFF);
}

vec3 random(vec3 min, vec3 max) {
    return {
        random(min.x, max.x),
        random(min.y, max.y),
        random(min.z, max.z)
    };
}

static uint16 monkey_id{0};

void make_body(Pose T, vec3 vel) {
    Model& model = World::models.insert();
    model.mesh = monkey_id;
    model.pose = T;
    float Z = random() > .5 ? -1 : 1;
    model.metadata = vec4(Z==-1?0:1,0,Z==1?0:1,0);

    bodies.push({model.id, vel, model.pose, Z});
}

void Perekop::on_step(double dt) {
    const float pi = 3.14159265f;
    Game::step::camera(dt);

    t += dt;

    float r = sin(t), g = sin(t - pi * (2.f/3.f)), b = sin(t - pi * (4.f/3.f));
    r = (r+1)*.5; g = (g+1)*.5; b = (b+1)*.5;

    World::bgcol = {r, g, b};

    for (int i = 0; i < bodies.size(); i++) {
        body &ibody = bodies[i];
        if (i != bodies.size() - 1)
            for (int j = i + 1; j < bodies.size(); j++) {
                body &jbody = bodies[j];
                vec3 disp = jbody.pos - ibody.pos;
                float r2 = dot(disp, disp);
                float r = sqrt(r2);
                vec3 force;
                if (r > 1)
                    force = disp * ( -ibody.Z * jbody.Z * (float)dt / (r2 * r));
                else {
                    force = -disp;
                }


                ibody.vel += force;
                jbody.vel -= force;
            }
        
        ibody.pos += ibody.vel * (float)dt;
        ibody.vel *= 0.999f;
        World::models[ibody.modelid].pose.pos = ibody.pos;
    }
}

void Perekop::on_launch() {
    Game::init::camera();

    printf("Game begin\n");

    Shader& chudshader = *new Shader("chudshader",
        "game/assets/shaders/vert.glsl",
        "game/assets/shaders/frag.glsl"
    );

    Texture chudtexture = Texture("game/assets/images/ourbrainsareshrinking.jpg");

    chudshader.uniform(UniformType::u_float, "time", &t);
    chudshader.uniform(UniformType::u_vec3, "f_bgcol", &World::bgcol);

    Mesh& monkey = World::meshes.insert();
    file::read_obj("game/assets/models/truemesh.obj", monkey);

    monkey.shader = &chudshader;
    monkey.texture = chudtexture;

    for (int i = 0; i < 2000; i++) {
        make_body(random({-25, -25, -25}, {25, 25, 25}), {});
    }
}

void Perekop::on_exit() {
    printf("game's gone\n");
} 