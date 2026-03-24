#include "engine.hpp"
#include <iostream>

using namespace pk;

void frame(float dt) {
    std::cout << "new frame?!\n";
}


int main() {
    engine::init();

    auto bcon = engine::window::began.connect([](int i){ std::cout << "began!\n"; });
    auto econ = engine::window::ended.connect([](int i){ std::cout << "ended?\n"; });
    auto rcon = engine::window::resized.connect([](glm::vec2 newsize){ std::cout << "resized to " << newsize.x << ", " << newsize.y << "\n"; });
    auto scon = engine::window::step.connect(frame);
    return 0;
}