#pragma once
#include <limits>
#include <PK/Math/typedef.hpp>
#include <PK/Core/vector.hpp>
#include <initializer_list>
#include <algorithm>
#include <PK/Core/algorithm.hpp>

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
                    min = std::min(min, v.t);
                    max = std::max(max, v.t);
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
                        l = std::exchange(x,(h+x)>>1);
                    else
                        h = std::exchange(x, (l+x)>>1);
                }
                
                if (x >= values.size() - 1) return values.back().x;

                auto& va = values[x]; auto &vb = values[x+1];
                float alpha = (t-va.t) / (vb.t-va.t);

                return va.x + (vb.x - va.x) * alpha;
            }
        };
}