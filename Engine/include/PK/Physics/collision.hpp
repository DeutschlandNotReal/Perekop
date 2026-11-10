#pragma once

#include <PK/Physics/Body.hpp>
#include <PK/Core/set.hpp>

namespace pk::physics {
    struct colContacts {
        vec3 normal{0.f};
        array<vec3, 8> points{};
        uint count{0};
    };

    colContacts getContacts(const Body&, const Body&, vec3 normal) noexcept;
    
    void collide(Body& A, Body& B, float dt) noexcept;
}
