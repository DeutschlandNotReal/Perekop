#include <Perekop.hpp>
#include <pkutil/File.hpp>
#include <game/freecam.hpp>

using namespace pk;
using namespace glm;
using namespace Perekop;
static float t = 0;

struct body { int modelid; vec3 vel, pos; };

static Array<body> bodies;
static uint fake_randomler = 0;

uint random() {
    fake_randomler ^= 0x123123F;
    fake_randomler += 0x100041F;
    fake_randomler *= 0x004112F;
    fake_randomler ^= 0x231231F;
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
    for (int i = 0; i < bodies.size(); i++) {
        body& islop = bodies[i];
        if (i != bodies.size() - 1)
            for (int j = i + 1; j < bodies.size(); j++) {
                body& jslop = bodies[j];
                vec3 disp = jslop.pos - islop.pos;
                float r2 = dot(disp, disp);
                if (r2 > 400 || r2 < 1) continue;
                float r = sqrt(r2);
                vec3 force = disp * ((3.f-r) * (float)dt * .15f / r2);
                islop.vel += force;
                jslop.vel -= force;
            } 
        islop.pos += islop.vel * (float)dt;
        islop.vel *= 0.98f;
        World::models[islop.modelid].pose.pos = islop.pos;
    }
}

void Perekop::on_launch() {
    World::camera.pose.pos = {50, 50, 150};
    Game::freecam_init();
    std::cout << "Game begin\n";

    const char* vsrc = File::read("game/shaders/vert.glsl");
    const char* fsrc = File::read("game/shaders/frag.glsl");
    auto* chudmat = new Mesh::Material(vsrc, fsrc);
    chudmat->uniform(Mesh::u_float, "t", &t);
    delete[] vsrc;
    delete[] fsrc;

    Mesh& shapeler = World::meshes.insert();
    shapeler.mat = chudmat;

    shapeler.vertex = {
        {{ 0,  1,  0}, {0, 1, 0}, {.5, 1}},
        {{ 0, -1,  0}, {0,-1, 0}, {.5, 0}},
        {{ 1,  0,  1}, {1, 0, 1}, {1, .5}},
        {{-1,  0,  1}, {-1,0, 1}, {0, .5}},
        {{-1,  0, -1}, {-1,0,-1}, {0, .5}},
        {{ 1,  0, -1}, {1, 0,-1}, {1, .5}}
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
    for (int i = 0; i < 30; i++) {
        int id = World::models.insert().id;
        shapeler.models.push(id);
        bodies.push({
            id,
            random({-1, -1, -1}, {1, 1, 1}),
            random({0, 0, 0}, {100, 100, 100})
        });
    }
}

void Perekop::on_exit() {
    std::cout << "game's gone\n";
} 