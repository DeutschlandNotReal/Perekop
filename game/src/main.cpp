#include "engine.hpp"
#include <iostream>

using namespace pk;
using namespace pk::engine;

int main() {
    window::began << [](){
        std::cout << "frame began?!";
    };

    window::ended << []() {
        std::cout << "frame ended...";
    };

    window::resized << [](auto size) {
        std::cout << "resized to " << size.x << ", " << size.y;
    };

    window::step << [](auto dt) {
        std::cout << "new frame (dt: " << dt << ")";
    };

    init();
    // anything after here wont work until window closed

    return 0;
}