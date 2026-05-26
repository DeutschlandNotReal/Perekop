#include <iostream>

#include <pk/Engine.hpp>
#include <pkutil/File.hpp>
#include <game/freecam.hpp>

using namespace pk;
using namespace glm;
static float t = 0;

struct body { int modelid; vec3 vel, pos; };

static pk::Array<body> bodies;

static unsigned int fake_randomler = 0;
unsigned int random() {
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
    return min + (max - min) * vec3(
        random() / (float)0xFFFFFFFF,
        random() / (float)0xFFFFFFFF,
        random() / (float)0xFFFFFFFF
    );
}

void Perekop::on_step(double dt) {
    Game::freecam_step(dt);

    for (int i = 0; i < bodies.size(); i++) {
        body& islop = bodies[i];
        for (int j = i; j < bodies.size()-1; j++) {
            body& jslop = bodies[j];
            vec3 disp = jslop.pos - islop.pos;
            float r = length(disp);
            if (r > 20 || r < 1.f) continue;
            vec3 force = (float)dt * disp * (1.f - 4.f/r);
            islop.vel += force;
            jslop.vel -= force;
        } 
        islop.pos += islop.vel * (float)dt;
        islop.vel *= 0.98f;
        scene.models[islop.modelid].pose.pos = islop.pos;
    }
}

void Perekop::on_launch() {
    Game::freecam_init();
    std::cout << "Game begin\n";

    const char* vsrc = File::read("game/shaders/vert.glsl");
    const char* fsrc = File::read("game/shaders/frag.glsl");
    auto* chudmat = new Mesh::Material(vsrc, fsrc);
    delete[] vsrc;
    delete[] fsrc;

    Mesh& cubeler = scene.meshes.insert();
    cubeler.mat = chudmat;
    cubeler.vertex = {
        {{0, 0, 0}, {0, 0, 0}, {0, 0}},
        {{1, 0, 0}, {0, 0, 0}, {1, 0}},
        {{1, 1, 0}, {0, 0, 0}, {1, 1}},
        {{0, 1, 0}, {0, 0, 0}, {0, 1}},
        {{0, 0, 1}, {0, 0, 0}, {0, 0}},
        {{1, 0, 1}, {0, 0, 0}, {1, 0}},
        {{1, 1, 1}, {0, 0, 0}, {1, 1}},
        {{0, 1, 1}, {0, 0, 0}, {0, 1}}
    };

    for (Mesh::Vertex& v : cubeler.vertex)
        v.pos = glm::normalize(v.pos) * random(6, 10);
    cubeler.radialize();

    chudmat->add_uniform(Mesh::u_float, "t", &t);

    cubeler.indices = {
        0,2,1, 0,3,2, // front
        4,5,6, 4,6,7, // back
        0,1,5, 0,5,4, // bottom
        1,2,6, 1,6,5, // right
        2,3,7, 2,7,6, // top
        3,0,4, 3,4,7  // left
    };

    cubeler.refresh();
    for (int x = -6; x < 6; x++) 
        for (int y = -6; y < 6; y++) 
            for (int z = -3; z < 3; z++) {
                Model& model = scene.models.insert();
                scene.link(cubeler.id, model.id);
                bodies.push({model.id, random({-.5f, .5f, .5f}, {.5f, .5f, .5f}), vec3{x, y, z} * 10.f});
            }
}

void Perekop::on_close() {
    std::cout << "game's gone\n";
} 