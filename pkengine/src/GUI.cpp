#include "Perekop.hpp"
#include <pk/GUI.hpp>
#include <Internal.hpp>
using namespace pk;
using namespace glm;

void Perekop::get_gui_top() {
    if (GUI::gui.size() == 0) return;
    vec2 point = Mouse::pos / Window::size;
    GUI::top = nullptr;

    for (GUIObject& gui : GUI::gui) {
        bool occupied = gui.intersect(point);
        if (occupied) {
            if (!GUI::top || GUI::top->Z > gui.Z) GUI::top = &gui;
            if (!gui.entered) { gui.entered = true; GUI::on_enter.fire(&gui); }
        } else if (gui.entered) {
            gui.entered = false;
            GUI::on_exit.fire(&gui);
        }
    }
}