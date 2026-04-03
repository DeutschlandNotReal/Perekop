#pragma once

#include <Perekop/Thread.hpp>

namespace PKInternal::Workers {
   inline pk::WorkPool render;
   inline pk::WorkPool game;

   inline void init() {
      render.start();
      game.start();
   }
}