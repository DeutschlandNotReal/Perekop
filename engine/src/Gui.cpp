#define PK_ENGINE_SRC
// #define PK_DEBUG "gui.cpp"

#include <Internal.hpp>
#include <PKLib/Gui.hpp>



using namespace pk;
using namespace glm;

void Perekop::query_gui() {
    Gui::top = nullptr;
    if (Gui::items.empty()) return;
    for (GUIObject& gui : Gui::items) {
        gui.entered = gui.intersect(Mouse::pos);
        if (gui.entered && (!Gui::top || Gui::top->Z > gui.Z))
            Gui::top = &gui; 
    }
}