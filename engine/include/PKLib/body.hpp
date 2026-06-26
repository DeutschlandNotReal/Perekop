#pragma once
#include <PKCore/slot_map.hpp>
#include <PKLib/model.hpp>

#ifdef PK_INTERNAL
namespace Perekop { void render(bool); void step_physics(f32); }
#endif

namespace pk {
    class Body {
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        friend void Perekop::step_physics(f32);
        #endif
        friend slot_map<Body>;

        mat3 I{1}, invI{1};
        f32 mass{0};
        bool dirty{false};
        uint16_t rootid{0}; // root model

        vec3 force{0}, torque{0};
        mat3 inverse_inertia();

        Body(Model& root);
        public:
            Pose pose;
            vec3 vel{0}, angvel{0};
            u16 id;

            void add_model(Model& model, f32 mass);

            void impulse(vec3 p, vec3 F);
            void impulse_local(vec3 r, vec3 F);

            void add_mass(vec3 p, f32 m);
            f32 get_mass() { return mass; }
    };
}