#pragma once
#include <PK/Physics/model.hpp>
#include <PK/Render/mesh.hpp>

namespace pk::Physics { 
    mat3 GetUniformInertia(
        span<const Mesh::Vertex> vertices
    ) noexcept;

    class Body {
        int flags{0};

        public:
            enum class StateFlag: int {
                anchored, weightless
            };

            bool GetFlag(StateFlag F) noexcept {
                return (flags >> (int)F) & 1;
            }

            void SetFlag(StateFlag F, bool v) noexcept {
                if (v) flags |= 1 << (int)F;
                else flags &= ~(1 << (int)F);
            }
        
            Model model; // inertia is per-mesh
            float mass{1.f};
            vec3 vel{0.f}, avel{0.f};
            vec3 force{0.f}, moment{0.f};

            void Impulse(
                vec3 localforce
            ) noexcept;

            void Impulse(
                vec3 localforce, vec3 localpoint
            ) noexcept;
    };

    inline float gravity = 24.f;
}