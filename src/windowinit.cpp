#include <PK/Render/window.hpp>
#include <PK/tick.hpp>
#include <PK/world.hpp>

#include "./functions.hpp"

using namespace pk;

void PKG::onInit::window() {
    Window::main.setIcon(world::assets/"images/icon.png");
}