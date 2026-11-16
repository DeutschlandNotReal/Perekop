#pragma once
#include <PK/Physics/ModelBVH.hpp>
#include <PK/Render/Resources/shader.hpp>

namespace Perekop {
    inline pk::ModelBVH ModelBVH;

    namespace init { void drawer(); void render(); }
    namespace step { void drawer(float dt); void physics(float dt); }
}