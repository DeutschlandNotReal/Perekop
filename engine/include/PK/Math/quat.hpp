#pragma once 
#include <PK/Math/vec.hpp>
#include <PK/Math/mat.hpp>

namespace pk {
    struct quat {
        union { f32x4 val; struct {f32 x, y, z, w; }; };

        [[nodiscard]] constinl quat() = default;
        [[nodiscard]] constinl quat(f32 x, f32 y, f32 z, f32 w) noexcept: x{x}, y{y}, z{z}, w{w} {}
        [[nodiscard]] constinl quat(f32x4 val) noexcept: val{val} {}
        [[nodiscard]] constinl static quat identity() noexcept { return {0.f, 0.f, 0.f, 1.f}; }

        constinl quat& operator+=(quat b) noexcept { val += b.val; return *this; }
        constinl quat& operator-=(quat b) noexcept { val -= b.val; return *this; }
        constinl quat& operator*=(quat b) noexcept { 
            // hamiltonian prod
            // x = w0*x1 + x0*w1 + y0*z1 - z0*y1
            // y = w0*y1 - x0*z1 + y0*w1 + z0*x1
            // z = w0*z1 + x0*y1 - y0*x1 + z0*w1
            // w = w0*w1 - x0*x1 - y0*y1 - z0*z1

            f32x4 res = val.swizzle<0, 0, 0, 0>().negate<0, 1, 0, 1>() * b.val.swizzle<3, 2, 1, 0>(); 
            res += val.swizzle<1, 1, 1, 1>().negate<0, 0, 1, 1>() * b.val.swizzle<2, 3, 0, 1>();
            res += val.swizzle<2, 2, 2, 2>().negate<1, 0, 0, 1>() * b.val.swizzle<1, 0, 3, 2>();
            res += val.swizzle<3, 3, 3, 3>() * b.val;

            return *this = res;
        }

        // axis must be unit!!
        [[nodiscard]] constinl static quat axis_angle(vec3 axis, f32 angle) noexcept {
            f32 sinA, cosA; angle *= .5f;
            if consteval {
                sinA = std::sin(angle); 
                cosA = std::cos(angle); 
            } else {
                sincosf(angle, &sinA, &cosA);
            }
            return {axis.x * sinA, axis.y * sinA, axis.z * sinA, cosA};
        }

        [[nodiscard]] constinl quat conjugate() const noexcept {
            return val.negate<1, 1, 1, 0>();
        }

        [[nodiscard]] constinl quat inverse() const noexcept {
            // q' / |q|^2
            return val.negate<1, 1, 1, 0>() * val.dot().rcp();
        }

        [[nodiscard]] constinl operator mat3() const noexcept {
            f32 xx = x*x, yy = y*y, zz = z*z;
            f32 xy = x*y, xz = x*z, yz = y*z;
            f32 wx = w*x, wy = w*y, wz = w*z;

            return {
                1 - 2*(yy+zz), 2*(xy+wz), 2*(xz-wy),
                2*(xy-wz), 1 - 2*(xx+zz), 2*(yz+wx),
                2*(xz+wy), 2*(yz-wx), 1 - 2*(xx+yy)
            };
        }
        
    };

    constinl quat operator+(quat a, quat b) noexcept { return a+=b; }
    constinl quat operator-(quat a, quat b) noexcept { return a-=b; }
    constinl quat operator*(quat a, quat b) noexcept { return a*=b; }

    constinl vec4 operator*(quat q, vec4 v) noexcept {
        // t = 2( q.xyz x v )
        // q * v = v + wt + q.xyz x t
        f32x4 q_yzxw = q.val.swizzle<1, 2, 0, 3>();
        f32x4 q_zxyw = q.val.swizzle<2, 0, 1, 3>();

        f32x4 t = 2.f * ( q_yzxw * v.val.swizzle<2, 0, 1, 3>() - q_zxyw *  v.val.swizzle<1, 2, 0, 3>());
        
        return v + vec4(t * q.val.swizzle<3, 3, 3, 3>() + ( q_yzxw * t.swizzle<2, 0, 1, 3>() - q_zxyw * t.swizzle<1, 2, 0, 3>()));
    }
}