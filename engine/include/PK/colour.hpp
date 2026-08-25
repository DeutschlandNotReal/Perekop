#pragma once

namespace pk {
    using byte = unsigned char;

    struct colour {
        byte R, G, B, A;

        constexpr colour() noexcept = default;

        template <typename RT, typename GT, typename BT, typename AT>
        constexpr colour(RT R, GT G, BT B, AT A = 255) noexcept: 
            R(static_cast<byte>(R)),
            G(static_cast<byte>(G)),
            B(static_cast<byte>(B)),
            A(static_cast<byte>(A))
        {}

        constexpr colour(unsigned int RGBA) noexcept {
            *(unsigned int*)&R = RGBA;
        }

        constexpr colour& operator*=(float scl) noexcept {
            R *= scl; B *= scl; G *= scl; A *= scl;
            return *this;
        }

        constexpr colour& operator+=(colour b) noexcept {
            R += b.R; G += b.G; B += b.B; A += b.A;
            return *this;
        }

        constexpr colour& operator-=(colour b) noexcept {
            R -= b.R; G -= b.G; B -= b.B; A -= b.A;
            return *this;
        }

        constexpr colour operator-() const noexcept {
            return {255-R, 255-G, 255-B, 255-A};
        }
    };

    [[nodiscard]] constexpr colour operator*(colour a, float scl) noexcept { return a *= scl; }
    [[nodiscard]] constexpr colour operator+(colour a, colour b)  noexcept { return a += b; }
    [[nodiscard]] constexpr colour operator-(colour a, colour b)  noexcept { return a -= b; }

    inline constexpr colour blend(colour a, colour b, float t) noexcept {
        return a * t + b * (1.f - t);
    }
}