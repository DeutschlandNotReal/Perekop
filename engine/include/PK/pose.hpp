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
        [[nodiscard]] pose(mat4 mat) noexcept: pos(mat[3]), rot(quat_cast(mat3(mat))) {}
        [[nodiscard]] pose(mat3 rot) noexcept: pos(0), rot(quat_cast(rot)) {}

        [[nodiscard]] pose Inverse() const noexcept {
            quat r = conjugate(rot);
            return {-(r * pos), r};
        }

        [[nodiscard]] operator mat4() const noexcept {
            mat4 m{mat3_cast(rot)};
            m[3] = vec4(pos, 1);
            return m;
        }

        [[nodiscard]] operator mat3() const noexcept { 
            return mat3_cast(rot);
        }

        [[nodiscard]] vec3 Right() const noexcept { return rot * vec3(1, 0, 0); }
        [[nodiscard]] vec3 Up() const noexcept { return rot * vec3(0, 1, 0); }
        [[nodiscard]] vec3 Forward() const noexcept { return rot * vec3(0, 0,-1); }

        [[nodiscard]] static pose LookAt(vec3 from, vec3 at, vec3 up = {0, 1, 0}) noexcept {
            return {from, quatLookAt(normalize(at - from), up)};
        }

        [[nodiscard]] static pose LookAlong(vec3 from, vec3 along, vec3 up = {0, 1, 0}) noexcept {
            return {from, quatLookAt(along, up)};
        }
        
        [[nodiscard]] static pose FromEuler(vec3 pos, vec3 euler) noexcept {
            return {pos, euler};
        }

        [[nodiscard]] vec3 EulerAngles() const noexcept {
            return glm::eulerAngles(rot);
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

    [[nodiscard]] inline vec3 worldspace(vec3 v, pose space) noexcept { return space.rot * v + space.pos; }
    [[nodiscard]] inline vec3 worldspace(vec3 v, quat space) noexcept { return space * v; }
    [[nodiscard]] inline vec3 worldspace(vec3 v, vec3 space) noexcept { return space + v; }

    [[nodiscard]] inline vec3 localspace(vec3 v, pose space) noexcept { return conjugate(space.rot) * (v - space.pos); }
    [[nodiscard]] inline vec3 localspace(vec3 v, quat space) noexcept { return conjugate(space) * v; }
    [[nodiscard]] inline vec3 localspace(vec3 v, vec3 space) noexcept { return v - space; }

    [[nodiscard]] inline vec3 operator*(vec3 v, pose p) noexcept { return localspace(v, p); }
}