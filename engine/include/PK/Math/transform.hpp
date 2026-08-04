#pragma once
#include <PK/Math/quat.hpp>
#include <PK/Math/mat.hpp>

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
        [[nodiscard]] constinl vec4 vec_lspace(vec4 v) const noexcept {
            return rot.conjugate() * v;
        }

        //vector to world space
        [[nodiscard]] constinl vec4 vec_wspace(vec4 v) const noexcept {
            return rot * v;
        }

        // point to local space
        [[nodiscard]] constinl vec4 point_lspace(vec4 v) const noexcept {
            return vec_lspace(v) + pos;
        }

        // point to world space
        [[nodiscard]] constinl vec4 point_wspace(vec4 v) const noexcept {
            return vec_wspace(v - pos);
        }

        [[nodiscard]] inline constexpr mat4 matrix() const noexcept {
            f32 xx = rot.x*rot.x, yy = rot.y*rot.y, zz = rot.z*rot.z;
            f32 xy = rot.x*rot.y, xz = rot.x*rot.z, yz = rot.y*rot.z;
            f32 wx = rot.w*rot.x, wy = rot.w*rot.y, wz = rot.w*rot.z;

            return {
                1 - 2*(yy+zz), 2*(xy+wz), 2*(xz-wy), pos.x,
                2*(xy-wz), 1 - 2*(xx+zz), 2*(yz+wx), pos.y,
                2*(xz+wy), 2*(yz-wx), 1 - 2*(xx+yy), pos.z,
                0, 0, 0, 1
            };
        }

        [[nodiscard]] inline constexpr mat4 invmatrix() const noexcept {
            transform inv = *this;
            inv.rot = inv.rot.conjugate();
            inv.pos = inv.rot * -inv.pos;

            return inv.matrix();
        }
    };
 
    [[nodiscard]] constinl transform operator+(transform t, vec4 v) noexcept { return t += v; }
    [[nodiscard]] constinl transform operator-(transform t, vec4 v) noexcept { return t -= v; }
    [[nodiscard]] constinl transform operator*(transform t, vec4 v) noexcept { return t *= v; }
    [[nodiscard]] constinl transform operator/(transform t, vec4 v) noexcept { return t /= v; }
    [[nodiscard]] constinl transform operator*(transform t, quat q) noexcept { return t *= q; }
    [[nodiscard]] constinl transform operator*(transform t, const transform& b) noexcept { return t *= b; }
}