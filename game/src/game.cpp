#include "engine.hpp"
#include <iostream>
using namespace pk::engine;
using glm::vec3;

float theta = 0;

inline glm::vec3 cosxy(float theta) {
    glm::vec3 pos;
    sincosf(glm::radians(theta), &pos.x, &pos.z);
    return pos;
}

void game::launch() {
    std::cout << "game's so back\n";
    float angle = 0;

    Mesh* pyramidler = scene::new_mesh();
    ID_T top0  = pyramidler->push_vertex(0, 1, 0);
    ID_T bot0 = pyramidler->push_vertex(cosxy(0));
    ID_T bot1 = pyramidler->push_vertex(cosxy(120));
    ID_T bot2 = pyramidler->push_vertex(cosxy(240));
    pyramidler->push_triangle(bot0, bot1, bot2);
    pyramidler->push_triangle(top0, bot1, bot2);
    pyramidler->push_triangle(top0, bot2, bot0);
    pyramidler->push_triangle(top0, bot0, bot1);

    Model* mod = new Model(pyramidler);

    window::step.connect([mod, &angle](double dt){
        mod->look_at(cosxy(angle += dt * 0.05), glm::vec3(0, 1, 0));
        return true;
    });
}

void game::close() {
    std::cout << "game's gone\n";
}