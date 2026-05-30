#include "Perekop.hpp"
#include <cstdio>
#include <pk/GUI.hpp>
#include <Internal.hpp>
using namespace pk;
using namespace glm;

GUIObject::GUIObject(GUIType T, float Z, glm::vec2 p, glm::vec2 s, glm::vec4 col):
    type(T), Z(Z), position(p), size(s), colour(col) {
        // soon...
}

inline bool boxtest(vec2 low, vec2 high, vec2 test) {
    printf("low(%.2f,%.2f), high(%.2f,%.2f), test(%.2f,%.2f)\n", low.x, low.y, high.x, high.y, test.x, test.y);
    return test.x >= low.x && test.y >= low.y && test.x <= high.x && test.y <= high.y;
}

static GUIObject* top{nullptr};

bool Perekop::gui_test_down(int k) {
    if (top) GUI::on_down.fire(top, k);
    return top == nullptr;;
}

bool Perekop::gui_test_up(int k) {
    if (top) GUI::on_up.fire(top, k);
    return top == nullptr;
}
bool Perekop::gui_test_move() {
    if (GUI::gui.size() == 0) return false;;
    vec2 mpos = Mouse::position() / Window::size();
    mpos.y = 1-mpos.y;
    GUIObject* min{nullptr};
    top = nullptr;

    for (GUIObject& gui : GUI::gui) {
        bool inside = boxtest(gui.position, gui.position+gui.size, mpos);
        if (inside) {
            if (!top || top->Z > gui.Z) top = &gui;
            if (!(gui.tflags & GUIObject::entered)) {
                gui.tflags |= GUIObject::entered;
                GUI::on_enter.fire(&gui);    
            }
        } else if (!inside && (gui.tflags & GUIObject::entered)) {
            gui.tflags &= ~GUIObject::entered;
            GUI::on_exit.fire(&gui);
        }
    }
    top = min;
    if (min && !(min->tflags & GUIObject::entered)) {
        // hasnt entered before
        min->tflags |= GUIObject::entered;
        GUI::on_enter.fire(min);
    }
    return (top != nullptr);
}