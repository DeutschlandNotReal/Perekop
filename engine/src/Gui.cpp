#define PK_ENGINE_SRC
#include <Internal.hpp>

using namespace pk;
using namespace glm;

void Perekop::query_gui() {
    Gui::top = nullptr;

    for (gui_instance &gui : Gui::items) {
        gui.entered = gui.is_intersecting(Mouse::pos);
        if (gui.entered && (!Gui::top || Gui::top->Z > gui.Z))
            Gui::top = &gui; 
    }
}