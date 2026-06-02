#include <Internal.hpp>
#include <pk/Gui.hpp>

using namespace pk;
using namespace glm;

void Perekop::query_gui() {
    Gui::top = nullptr;
    if (Gui::items.empty()) return;
    for (GUIObject& gui : Gui::items) {
        bool occupied = gui.intersect(Mouse::pos);
        if (occupied) {
            if (!Gui::top || Gui::top->Z > gui.Z) Gui::top = &gui;
            if (!gui.entered) { gui.entered = true; Gui::on_enter.fire(&gui); }
        } else if (gui.entered) {
            gui.entered = false;
            Gui::on_exit.fire(&gui);
        }
    }
}