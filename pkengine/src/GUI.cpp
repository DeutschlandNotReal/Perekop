#include <Internal.hpp>
#include <pk/GUI.hpp>

using namespace pk;
using namespace glm;

void Perekop::query_gui() {
    GUI::top = nullptr;
    if (GUI::gui.size() == 0) return;
    for (GUIObject& gui : GUI::gui) {
        bool occupied = gui.intersect(Mouse::pos);
        if (occupied) {
            if (!GUI::top || GUI::top->Z > gui.Z) GUI::top = &gui;
            if (!gui.entered) { gui.entered = true; GUI::on_enter.fire(&gui); }
        } else if (gui.entered) {
            gui.entered = false;
            GUI::on_exit.fire(&gui);
        }
    }
}