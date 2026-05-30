#include <Perekop.hpp>
#include <game/gui.hpp>

using namespace pk;
using namespace glm;
using namespace Perekop;

void Game::gui_init() {
    GUI::gui.push({0xFFFFFFFF, {.3, .3}, {.1, .1}, 1});
    GUI::gui.push({0xFF4421FF, {.2, .2}, {0.2 , 0}, 2});
    
    Mouse::on_move.listen([](vec2 movement){
        if (GUI::top && Mouse::held(0)) {
            GUI::top->pos += movement / Window::size;
            GUI::top->colour ^= 0xFF12FF00;
        }
    });
}