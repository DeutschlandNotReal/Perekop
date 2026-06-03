#pragma once
#include <pkutil/Array.hpp>
#include <pkutil/Set.hpp>
#include <pk/Geometry.hpp>

#ifdef PK_INTERNAL
namespace Perekop { void render(bool); }
#endif

namespace pk {
    class Body {
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif
        friend Set<Body>;

        glm::mat3 I{1}, invI{1};
        float mass{1};
        bool dirty{false};
        short rootid{0}; // root model

        Body(Model& root);
        public:
            Pose pose;
            glm::vec3
                vel{0},
                force{0},
                avel{0},
                torque{0};
            void intergrate(const float& dt);
            short id;

            void add_mass(float m);
            void add_pmass(glm::vec3 p, float m);
            float get_mass() { return mass; }

            void add_model(Model& model, float mass);
            void apply_force(glm::vec3 p, glm::vec3 F);
            void apply_lforce(glm::vec3 r, glm::vec3 F);
            glm::mat3 inverse_inertia();
    };
}