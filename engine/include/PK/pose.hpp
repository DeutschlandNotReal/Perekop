#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace glm;
namespace pk {
    struct pose {
        vec3 pos{0}; 
        quat rot{1,0,0,0};

        [[nodiscard]] pose() noexcept = default;

        [[nodiscard]] pose(vec3 pos) noexcept: pos(pos) {}
        [[nodiscard]] pose(vec3 pos, quat rot) noexcept: pos(pos), rot(rot) {}

        [[nodiscard]] vec3 localspace(vec3 v) const noexcept {
            return rot * v + pos;
        }

        [[nodiscard]] vec3 worldspace(vec3 v) const noexcept {
            return conjugate(rot) * (v - pos);
        }

        [[nodiscard]] mat4 invmat4() const noexcept {
            quat inv = conjugate(rot);
            return translate(identity<mat4>(), -(inv * pos)) * mat4_cast(inv);
        }

        [[nodiscard]] operator mat4() const noexcept {
            return translate(identity<mat4>(), pos) * mat4_cast(rot);
        }
 
        pose& operator+=(vec3 v) noexcept { pos += v; return *this; }
        pose& operator-=(vec3 v) noexcept { pos -= v; return *this; }
        pose& operator*=(quat r) noexcept { rot *= r; return *this; }
        pose& operator*=(pose p) noexcept { pos += rot * p.pos; rot *= p.rot; return *this; }
    };

    [[nodiscard]] inline pose operator+(pose p, vec3 v) noexcept { return p += v; }
    [[nodiscard]] inline pose operator-(pose p, vec3 v) noexcept { return p -= v; }
    [[nodiscard]] inline pose operator*(pose p, quat r) noexcept { return p *= r; }
    [[nodiscard]] inline pose operator*(pose a, pose b) noexcept { return a *= b; }
}