#pragma once
#include <PK/Math/quat.hpp>

namespace pk {    
    struct transform {
        vec4 pos; quat rot;

        [[nodiscard]] constinl transform() noexcept = default;
        [[nodiscard]] constinl transform(vec4 p) noexcept: pos(p), rot(quat::identity()) {}
        [[nodiscard]] constinl transform(vec4 p, quat r) noexcept: pos(p), rot(r) {}

        constinl operator vec4() const noexcept { return pos; }
        constinl operator quat() const noexcept { return rot; }

        constinl transform& operator+=(vec4 v) noexcept { pos += v; return *this; }
        constinl transform& operator-=(vec4 v) noexcept { pos -= v; return *this; }
        constinl transform& operator*=(vec4 v) noexcept { pos *= v; return *this; }
        constinl transform& operator/=(vec4 v) noexcept { pos /= v; return *this; }

        constinl transform& operator*=(quat q) noexcept { rot *= q; return *this; }
        constinl transform& operator*=(const transform& t) noexcept { pos += t.pos; rot *= t.rot; return *this; }

        // vector to local space
        constinl vec4 vec_lspace(vec4 v) const noexcept {
            return rot.conjugate() * v;
        }

        //vector to world space
        constinl vec4 vec_wspace(vec4 v) const noexcept {
            return rot * v;
        }

        // point to local space
        constinl vec4 point_lspace(vec4 v) const noexcept {
            return vec_lspace(v) + pos;
        }

        // point to world space
        constinl vec4 point_wspace(vec4 v) const noexcept {
            return vec_wspace(v - pos);
        }
    };

    [[nodiscard]] constinl transform operator+(transform t, vec4 v) noexcept { return t += v; }
    [[nodiscard]] constinl transform operator-(transform t, vec4 v) noexcept { return t -= v; }
    [[nodiscard]] constinl transform operator*(transform t, vec4 v) noexcept { return t *= v; }
    [[nodiscard]] constinl transform operator/(transform t, vec4 v) noexcept { return t /= v; }
    [[nodiscard]] constinl transform operator*(transform t, quat q) noexcept { return t *= q; }
    [[nodiscard]] constinl transform operator*(transform t, const transform& b) noexcept { return t *= b; }
}