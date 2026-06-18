#include <Perekop.hpp>
#include <PKGame/gui.hpp>

using namespace pk;
using namespace glm;
using namespace Perekop;

void pkgame::init::gui() {
    Gui::items = {
        {{.4, .6, .6, 1}, {.3, .3}, {.1, .1}, 1},
        {{.3, .3, .1, 1}, {.2, .2}, {0.2 , 0}, 2}
    };
    
    Mouse::on_move.listen([](const vec2& movement){
        if (Gui::top && Mouse::held(Mouse::left)) 
            Gui::top->pos += movement / Window::get_size();
    });
}