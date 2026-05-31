#include <Perekop.hpp>
#include <game/gui.hpp>

using namespace pk;
using namespace glm;
using namespace Perekop;

void Game::gui_init() {
    GUI::gui.push({{.4, .6, .6, 1}, {.3, .3}, {.1, .1}, 1});
    GUI::gui.push({{.3, .3, .1, 1}, {.2, .2}, {0.2 , 0}, 2});
    
    Mouse::on_move.listen([](vec2 movement){
        if (GUI::top && Mouse::held(Mouse::left)) {
            GUI::top->pos += movement / Window::size;
        }
    });
}