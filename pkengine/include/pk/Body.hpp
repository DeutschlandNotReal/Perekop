#pragma once
#include <pkutil/Array.hpp>
#include <pkutil/Set.hpp>
#include <pk/Geometry.hpp>

#ifdef PK_INTERNAL
namespace Perekop { void render(bool); void step_physics(float dt); }
#endif

namespace pk {
    class Body {
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        friend void Perekop::step_physics(float);
        #endif
        friend Set<Body>;

        glm::mat3 I{1}, invI{1};
        float mass{0};
        bool dirty{false};
        short rootid{0}; // root model

        glm::vec3 force{0}, torque{0};
        glm::mat3 inverse_inertia();

        Body(Model& root);
        public:
            Pose pose;
            glm::vec3 vel{0}, angvel{0};
            short id;

            void add_model(Model& model, float mass);

            void impulse(glm::vec3 p, glm::vec3 F);
            void impulse_local(glm::vec3 r, glm::vec3 F);

            void add_mass(glm::vec3 p, float m);
            float get_mass() { return mass; }
    };
}