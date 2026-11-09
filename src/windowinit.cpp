#include <PK/Render/window.hpp>
#include <PK/tick.hpp>
#include <PK/world.hpp>

#include "./functions.hpp"

using namespace pk;

void PKG::onInit::window() {
    pk::mainWindow.setIcon(world::assets/"images/icon.ico");
}