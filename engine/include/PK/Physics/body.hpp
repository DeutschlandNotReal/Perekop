#pragma once
#include <PK/Physics/model.hpp>

namespace pk {
    struct Body {
        transform t, vel, acc;
        // vel transform: {vel, avel}
        // acc transform: {acc, torque}
        
        f32 mass{0};

        void add_model(Model& model, f32 mass);
        void add_mass(vec4 pmass); // (x, y, z, mass)

        // always local
        void impulse(vec4 p, vec4 F);
    };
}