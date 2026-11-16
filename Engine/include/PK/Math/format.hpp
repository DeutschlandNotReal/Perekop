#pragma once
#include <format>
#include <PK/Math/pose.hpp>

template<length_t L, typename T, qualifier Q>
struct std::formatter<vec<L, T, Q>> : std::formatter<T> {
    auto format(const vec<L, T, Q>& v, std::format_context& ctx) const {
        const auto& formatter = static_cast<const std::formatter<T>&>(*this);
        auto out = ctx.out();

        out = std::format_to(out, "(");
        out = formatter.format(v[0], ctx);
  
        for (length_t i{1}; i < L; ++i) {
            out = std::format_to(out, ", ");
            out = formatter.format(v[i], ctx);
        }

        return std::format_to(out, ")");
    }
};

template<length_t x, length_t y, typename T, qualifier Q>
struct std::formatter<mat<x, y, T, Q>> : std::formatter<T> {
    auto format(const mat<x, y, T, Q>& mat, std::format_context& ctx) const {
        const auto& formatter = static_cast<const std::formatter<T>&>(*this);
        auto out = ctx.out();

        for (length_t c{0}; c<y; c++) {
            out = std::format_to(out, "\n[ ");
            out = formatter.format(mat[0][c], ctx);

            for (length_t i{1}; i < x; i++) {
                out = std::format_to(out, ", ");
                out = formatter.format(mat[i][c], ctx);
            }

            std::format_to(out, " ]");
        }
        
        return std::format_to(out, "\n");
    }
};

template <typename T, qualifier Q>
struct std::formatter<qua<T, Q>> : std::formatter<T> {
    auto format(const qua<T, Q>& q, std::format_context& ctx) const {
        const auto& formatter = static_cast<const std::formatter<T>&>(*this);
        auto out = ctx.out();

        out = std::format_to(out, "(");
        out = formatter.format(q[0], ctx);
  
        for (length_t i{1}; i < 3; ++i) {
            out = std::format_to(out, ", ");
            out = formatter.format(q[i], ctx);
        }

        return std::format_to(out, ")");
    }
};
