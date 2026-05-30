#include <Perekop.hpp>
#include <cstdio>
#include <pkutil/File.hpp>
#include <game/freecam.hpp>

using namespace pk;
using namespace glm;
using namespace Perekop;
static float t = 0;

struct body { int modelid; vec3 vel, pos; float Z; };

static Array<body> bodies;
static uint fake_randomler = 0;

uint random() {
    fake_randomler ^= 0xF123123FA;
    fake_randomler += 0xF2100041F;
    fake_randomler *= 0xA004112F;
    fake_randomler ^= 0x0231231F;
    return fake_randomler;
}

float random(float min, float max) {
    return min + (max - min) * (random() / (float)0xFFFFFFFF);
}

vec3 random(vec3 min, vec3 max) {
    return {
        random(min.x, max.x),
        random(min.y, max.y),
        random(min.z, max.z)
    };
}

void Perekop::on_step(double dt) {
    Game::freecam_step(dt);
    t += dt;
    for (int i = 0; i < bodies.size(); i++) {
        body& ibody = bodies[i];
        if (i != bodies.size() - 1)
            for (int j = i + 1; j < bodies.size(); j++) {
                body& jbody = bodies[j];
                vec3 disp = jbody.pos - ibody.pos;
                float r2 = dot(disp, disp);
                //if (r2 > 500 || r2 < .5) continue;
                float r = sqrt(r2);
                vec3 force = disp * (( 10.f-r) * -1.0f * ibody.Z * jbody.Z * (float)dt / (r2));
                ibody.vel += force;
                jbody.vel -= force;
            } 
        ibody.pos += ibody.vel * (float)dt;
        ibody.vel *= 0.9f;
        World::models[ibody.modelid].pose.pos = ibody.pos;
    }
}

void Perekop::on_launch() {
    Game::freecam_init();
    printf("Game begin\n");

    const char 
        *vsrc = File::read("game/assets/shaders/vert.glsl"),
        *fsrc = File::read("game/assets/shaders/frag.glsl");
    auto* chudmat = new Mesh::Material(vsrc, fsrc);
    chudmat->uniform(Mesh::u_float, "t", &t);
    delete[] vsrc;
    delete[] fsrc;

    Mesh& shapeler = World::meshes.insert();
    shapeler.mat = chudmat;

    shapeler.vertex = {
        {{ 0, 1, 0}, {0, 1, 0}, {.5, 1}},
        {{ 0,-1, 0}, {0,-1, 0}, {.5, 0}},
        {{ 1, 0, 1}, {1, 0, 1}, {1, .5}},
        {{-1, 0, 1}, {-1,0, 1}, {0, .5}},
        {{-1, 0,-1}, {-1,0,-1}, {0, .5}},
        {{ 1, 0,-1}, {1, 0,-1}, {1, .5}}
    };

    for (Mesh::Vertex& v : shapeler.vertex)
        v.pos = normalize(v.pos);

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

    shapeler.load();

    Window::gui.push({-4, {.1, .1}, {.56, .57}, {.6, .7, .8}});
    for (int i = 0; i < 100; i++) {
        Model& model = World::models.insert();
        shapeler.models.push(model.id);
        vec3 vel = random({-1, -1, -1}, {1, 1, 1}), 
             pos = random({0, 0, 0}, {10, 10, 10});
        float Z = random(0, 1) > 0.5 ? -1 : 1;
        model.pose.pos = pos; 
        model.metadata = vec4(Z==-1?0:1,0,Z==1?0:1,0);
        bodies.push({model.id, vel, pos, Z});
        //printf("V(%+.2f,%+.2f,%+.2f), P(%+.2f,%+.2f,%+.2f)\n", vel.x, vel.y, vel.z, pos.x, pos.y, pos.z);
    }
}

void Perekop::on_exit() {
    printf("game's gone\n");
} 