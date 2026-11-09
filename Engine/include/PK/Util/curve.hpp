#pragma once

#include <PK/Math/clamp.hpp>
#include <PK/Math/typedef.hpp>
#include <PK/Core/vector.hpp>
#include <PK/Core/algorithm.hpp>

#include <limits>
#include <initializer_list>
#include <algorithm>

namespace pk {
    template <typename T> class Curve {
        public:
            float min{std::numeric_limits<float>::infinity()}, max{-min};

            struct CurveEntry {
                T x; float t;
                bool operator>(CurveEntry b) noexcept { return t > b.t; }
            };

        private:
            vector<CurveEntry> values;

        public:
            Curve(std::initializer_list<CurveEntry> entries) {
                for (auto &v : entries) {
                    min = pk::min(min, v.t);
                    max = pk::max(max, v.t);
                    values.push(v);
                }

                span<CurveEntry> sortedValues{values};
                insertsort<CurveEntry>(sortedValues);
            }

            T operator[](float t) const noexcept {
                t = std::clamp(t, min, max);

                uint l{0}, h{values.size()}, x{h>>1};

                while (l != x) {
                    if (values[x].t < t) 
                        exchange(l, x,(h+x)>>1);
                    else
                        exchange(h, x, (l+x)>>1);
                }
                
                if (x >= values.size() - 1) return values.back().x;

                auto& va = values[x]; auto &vb = values[x+1];
                float alpha = (t-va.t) / (vb.t-va.t);

                return va.x + (vb.x - va.x) * alpha;
            }
        };
}