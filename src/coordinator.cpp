#include <PK/tick.hpp>
#include "./functions.hpp"

#include <PK/debug.hpp>

void Perekop::onInit() {
    for (auto &f : PKG::onInit::runorder) {
        // PK_NOTIF("onInit f()");
        f();
    }
}

void Perekop::onStep(float dt) {
    for (auto &f : PKG::onStep::runorder) { 
        f(dt);
    }
}

void Perekop::onExit() {
    /*
    for (auto &f : PKG::onExit::runorder) f();
    */
}