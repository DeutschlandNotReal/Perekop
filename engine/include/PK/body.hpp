#pragma once
#include <PK/model.hpp>

namespace pk {
    struct Body {
        pose pose, vel, acc;

        unsigned short id{0};
        float mass{0};

        void add_model(Model& model, float mass);
        void add_mass(vec4 pmass); // (x, y, z, mass)

        // always local
        void impulse(vec4 p, vec4 F);
    };
}