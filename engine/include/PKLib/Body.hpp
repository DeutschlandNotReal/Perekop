#pragma once
#include <PKCore/Set.hpp>
#include <PKLib/Geometry.hpp>

#ifdef PK_INTERNAL
namespace Perekop { void render(bool); void step_physics(float dt); }
#endif

namespace pk {
    class Body {
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        friend void Perekop::step_physics(float);
        #endif
        friend set<Body>;

        mat3 I{1}, invI{1};
        float mass{0};
        bool dirty{false};
        short rootid{0}; // root model

        vec3 force{0}, torque{0};
        mat3 inverse_inertia();

        Body(Model& root);
        public:
            Pose pose;
            vec3 vel{0}, angvel{0};
            short id;

            void add_model(Model& model, float mass);

            void impulse(vec3 p, vec3 F);
            void impulse_local(vec3 r, vec3 F);

            void add_mass(vec3 p, float m);
            float get_mass() { return mass; }
    };
}