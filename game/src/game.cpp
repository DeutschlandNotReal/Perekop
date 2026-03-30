#include <Perekop/Engine.hpp>
#include <iostream>
using namespace Perekop;
using namespace pk;

using glm::vec3;

inline glm::vec3 cosxy(float theta) {
    glm::vec3 pos;
    sincosf(glm::radians(theta), &pos.x, &pos.z);
    return pos;
}

void Game::launch() {
    std::cout << "game's so back\n";
    float angle = 0;

    Mesh* pyramidler = Scene::new_mesh();
    ID_T top0 = pyramidler->push_vertex(0, 1, 0);
    ID_T bot0 = pyramidler->push_vertex(cosxy(0));
    ID_T bot1 = pyramidler->push_vertex(cosxy(120));
    ID_T bot2 = pyramidler->push_vertex(cosxy(240));
    pyramidler->push_triangle(bot0, bot1, bot2);
    pyramidler->push_triangle(top0, bot1, bot2);
    pyramidler->push_triangle(top0, bot2, bot0);
    pyramidler->push_triangle(top0, bot0, bot1);

    pyramidler->load();
    pyramidler->flush();

    Model* mod = new Model(pyramidler);

    Scene::camera.origin = glm::vec3(5);
    Scene::camera.look = -Scene::camera.origin;
    Scene::camera.f = 100;

    Window::step.connect([mod, angle](double dt) mutable {
        angle += dt * 2.0f;
        mod->look_at(cosxy(angle), glm::vec3(0, 1, 0));
        return true;
    });

    Window::resized.connect([](int x, int y){
        std::cout << "true size framemog (" << x << ", " << y <<")\n";
        return true;
    });
}

void Game::close() {
    std::cout << "game's gone\n";
}  