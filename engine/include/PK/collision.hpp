#pragma once
#include <PK/pose.hpp>

namespace pk {
    struct shape; struct plane;
    struct ray { 
        vec3 pos, delta;
        ray(vec3 pos, vec3 delta) noexcept: pos(pos), delta(delta) {}
 
        static ray lookat(vec3 pos, vec3 to) noexcept { return {pos, to - pos}; }
    };

    struct plane { 
        vec3 pos, nor; 

        plane(vec3 pos, vec3 nor) noexcept: pos(pos), nor(nor) {}
        static plane lookat(vec3 from, vec3 at) noexcept { return {from, normalize(at - from)}; };
    };

    struct shape {
        enum Type: unsigned { Cuboid, Sphere };
        Type type{Cuboid};
        union {
            struct { vec3 extent; };
            struct { float r; };
        };

        constexpr shape() noexcept: extent{0} {}
    };

    [[nodiscard]] ray worldspace(ray, pose) noexcept;
    [[nodiscard]] ray worldspace(ray, vec3) noexcept;
    [[nodiscard]] ray localspace(ray, pose) noexcept;
    [[nodiscard]] ray localspace(ray, vec3) noexcept;
    
    [[nodiscard]] plane worldspace(plane, pose) noexcept;
    [[nodiscard]] plane worldspace(plane, vec3) noexcept;
    [[nodiscard]] plane localspace(plane, pose) noexcept;
    [[nodiscard]] plane localspace(plane, vec3) noexcept;

    namespace coltests {
        // disp/rel always second's position in first's localspace

        bool sphere( float r1, float r2, vec3 disp ) noexcept;
        bool aabb( vec3 extent1, vec3 extent2, vec3 disp ) noexcept;

        // BLOAT!! better use sphere or aabb before this
        bool obb( vec3 extent1, vec3 extent2, pose rel ) noexcept;

        bool obb_aabb( vec3 obbextent, vec3 aabbextent, pose rel ) noexcept;

        // works for AABB and OBB
        bool box_sphere( vec3 extent, float r, vec3 disp ) noexcept;

        bool ray_box( ray relray, vec3 extent ) noexcept;
        bool ray_sphere( ray relray, float r ) noexcept;

        // sphere -> AABB -> OBB
        bool obb_shortcut( vec3 extent1, vec3 extent2, pose rel ) noexcept;
    }

    bool intersects(shape, shape, pose relative) noexcept;
    bool intersects(shape, shape, vec3 relative) noexcept;
    bool intersects(shape, vec3 relative) noexcept;

    bool intersects(ray, shape, pose relative) noexcept;
    bool intersects(ray, shape, vec3 relative) noexcept;
    bool intersects(ray, plane) noexcept;

    bool intersects(plane, shape, pose relative) noexcept;
    bool intersects(plane, shape, vec3 relative) noexcept;
    bool intersects(plane, plane) noexcept;
}