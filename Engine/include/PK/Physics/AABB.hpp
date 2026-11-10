#pragma once
#include <PK/Math/pose.hpp>

namespace pk {
    struct AABB {
        vec3 min{0}, max{0};

        static constexpr AABB fromExtents(vec3 centre, vec3 ext) noexcept {
            return {.min = centre-ext, .max = centre+ext};
        }

        static constexpr AABB fromPoint(vec3 centre) noexcept {
            return {.min = centre, .max = centre};
        }

        static constexpr AABB combine(AABB i, AABB j) noexcept {
            return {.min = glm::min(i.min, j.min), .max = glm::max(i.max, j.max)};
        }

        static constexpr AABB overlap(AABB i, AABB j) noexcept {
            return {
                .min = glm::max(i.min, j.min), 
                .max = glm::min(i.max, j.max)
            };
        }

        bool occupies(vec3 p) const noexcept {
            return 
                max.x >= p.x && p.x >= min.x &&
                max.y >= p.y && p.y >= min.y &&
                max.z >= p.z && p.z >= min.z;
        }
        
        bool intersects(const AABB& other) const noexcept {
            return 
                min.x <= other.max.x && max.x >= other.min.x &&
                min.y <= other.max.y && max.y >= other.min.y &&
                min.z <= other.max.z && max.z >= other.min.z;
        }

        float volume() const noexcept { return (max.x - min.x) * (max.y - min.y) * (max.z - min.z); }

        vec3 getExtents() const noexcept { return .5f * (max-min); }
        vec3 getCentre() const noexcept { return .5f * (min+max); }
    };
}