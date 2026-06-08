#define PK_DEBUG "game.cpp"
#define PK_DEBUG_VEC   0
#define PK_DEBUG_SET   0
#define PK_DEBUG_MEM   0

#include <Perekop.hpp>
#include <cstdio>
#include <PKLib/File.hpp>
#include <game/camera.hpp>
#include <game/gui.hpp>

using namespace pk;
using namespace glm;
using namespace Perekop;
static float t = 0;

struct body { int modelid; vec3 vel, pos; float Z; };

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

void Perekop::on_step(double dt) {
    const float pi = 3.14159265f;
    Game::step::camera(dt);

    t += dt;

    float r = sin(t), g = sin(t - pi * (2.f/3.f)), b = sin(t - pi * (4.f/3.f));
    r = (r+1)*.5; g = (g+1)*.5; b = (b+1)*.5;

    World::bgcol = {r, g, b};

    for (int i = 0; i < bodies.size(); i++) {
        body& ibody = bodies[i];
        if (i != bodies.size() - 1)
            for (int j = i + 1; j < bodies.size(); j++) {
                body& jbody = bodies[j];
                vec3 disp = jbody.pos - ibody.pos;
                float r2 = dot(disp, disp);
                float r = sqrt(r2);
                vec3 force;
                if (r > 1)
                    force = disp * ( -ibody.Z * jbody.Z * (float)dt / (r2 * r));
                else 
                    force = -disp;

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
    //Game::init::gui();

    printf("Game begin\n");

    Shader& chudshader = *new Shader("chudshader",
        "game/assets/shaders/vert.glsl",
        "game/assets/shaders/frag.glsl"
    );

    Texture chudtexture = Texture("game/assets/images/ourbrainsareshrinking.jpg");

    chudshader.uniform(Uniform::u_float, "time", &t);
    chudshader.uniform(Uniform::u_vec3, "f_bgcol", &World::bgcol);

    Mesh& shapeler = World::meshes.insert();
    Mesh& pyramidler = World::meshes.insert();
    shapeler.shader = pyramidler.shader = &chudshader;
    shapeler.texture = pyramidler.texture = chudtexture;

    shapeler.vertices = {
        {{ 0, 1, 0}, {0, 1, 0}, {.5, 1}},
        {{ 0,-1, 0}, {0,-1, 0}, {.5, 0}},
        {{ 1, 0, 1}, {1, 0, 1}, {1, .5}},
        {{-1, 0, 1}, {-1,0, 1}, {0, .5}},
        {{-1, 0,-1}, {-1,0,-1}, {0, .5}},
        {{ 1, 0,-1}, {1, 0,-1}, {1, .5}}
    };

    shapeler.indices = { 
        0,2,3,
        0,3,4,
        0,4,5,
        0,5,2,
        1,3,2,
        1,4,3,
        1,5,4,
        1,2,5,
    };

    pyramidler.vertices = {
        {{ 0, 1, 0}, {0, 1, 0}, {0.5, 1}},
        {{-1, 0, -1}, {0, -1, 0}, {0, 0}},
        {{ 1, 0, -1}, {0, -1, 0}, {1, 0}},
        {{ 1, 0,  1}, {0, -1, 0}, {1, 1}},
        {{-1, 0,  1}, {0, -1, 0}, {0, 1}},
    };
 
    pyramidler.indices = {
    0, 1, 2,
    0, 2, 3,
    0, 3, 4,
    0, 4, 1,
    1, 2, 3,
    1, 3, 4
    };

    shapeler.load();
    pyramidler.load();

    for (int i = 0; i < 2000; i++) {
        Model& model = World::models.insert();
        model.mesh = (random(0,1)>0.5?shapeler:pyramidler).id;
        vec3 pos = random({-25, -25, -25}, {25, 25, 25});
        float Z = random(0, 1) > 0.5 ? -1 : 1;
        model.pose.pos = pos; 
        model.metadata = vec4(Z==-1?0:1,0,Z==1?0:1,0);
        bodies.push({model.id, {}, pos, Z});
        //printf("V(%+.2f,%+.2f,%+.2f), P(%+.2f,%+.2f,%+.2f)\n", vel.x, vel.y, vel.z, pos.x, pos.y, pos.z);
    }
}

void Perekop::on_exit() {
    printf("game's gone\n");
} 