#pragma once
#include <PK/pch.hpp>

namespace pk {
    class Camera {
        f32 tanfov, pfov;
        public:
            transform t;
            f32 min{.1f}, max{200.f};
            const f32& fov = pfov;

            constexpr void set_fov(f32 newfov) noexcept { 
                pfov = newfov;
                tanfov = std::tan(newfov * (pk::pi<> / 360.f)); // tan(fov radians / 2)
            }

            constexpr Camera() noexcept { set_fov(70.f); }

            constinl mat4 view() const noexcept { return t.invmatrix(); }

            constexpr mat4 proj(f32 width, f32 height) const noexcept {
                f32 rtfov = 1.f / tanfov; f32 rfmn = -1.f / (max - min);
                return {
                    height / width * rtfov, 0, 0, 0,
                    0, rtfov, 0, 0, 
                    0, 0, (max+min)*rfmn, 2*max*min*rfmn,
                    0, 0, -1, 0
                };
            }
    };
}