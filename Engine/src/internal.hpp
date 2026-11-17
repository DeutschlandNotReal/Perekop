#pragma once
#include <PK/Physics/ModelBVH.hpp>
#include <PK/Render/Resources/shader.hpp>

namespace Perekop {
    inline pk::ModelBVH ModelBVH;

    void initDrawer();
    void initRender();

    void stepDrawer();
    void stepPhysics(float);
}