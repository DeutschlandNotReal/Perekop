#pragma once

#include <PK/Physics/body.hpp>
#include <PK/Core/set.hpp>

namespace pk::physics {
    struct colContacts {
        vec3 normal{0.f};
        array<vec3, 8> points{};
        uint count{0};
    };

    colContacts getContacts(const body&, const body&, vec3 normal) noexcept;
    
    void collide(body& A, body& B, float dt) noexcept;
}
