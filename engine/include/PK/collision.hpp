#pragma once
#include <PK/pose.hpp>

namespace pk {
    struct ray { 
        vec3 pos, dir;
        ray() noexcept: pos{0}, dir{0,0,-1} {}
        ray(vec3 pos, vec3 dir) noexcept: pos{pos}, dir{dir} {}
        ray(pose pose) noexcept: pos{pose.pos}, dir{pose.fvec()} {}

        static ray lookAt(vec3 pos, vec3 at) noexcept { 
            return {pos, at - pos}; 
        }

        // values in local space
        float intersectCube(vec3 ext, vec3 localpos) const noexcept;
        float intersectBall(float r, vec3 localpos) const noexcept;
    };

    struct plane { 
        vec3 pos, nor; 
        plane(vec3 pos, vec3 nor = {0, 1, 0}) noexcept: pos(pos), nor(nor) {}
        
        static plane lookAt(vec3 from, vec3 at) noexcept { 
            return {from, normalize(at - from)}; 
        }
   
        float distance(vec3 point) const noexcept;
        vec3 project(vec3 point) const noexcept;
    };

    [[nodiscard]] ray worldspace(ray, pose) noexcept;
    [[nodiscard]] ray worldspace(ray, vec3) noexcept;
    [[nodiscard]] ray localspace(ray, pose) noexcept;
    [[nodiscard]] ray localspace(ray, vec3) noexcept;
    
    [[nodiscard]] plane worldspace(plane, pose) noexcept;
    [[nodiscard]] plane worldspace(plane, vec3) noexcept;
    [[nodiscard]] plane localspace(plane, pose) noexcept;
    [[nodiscard]] plane localspace(plane, vec3) noexcept;

    namespace overlapTests {
        bool SphereSphere(float r1, float r2, vec3 disp) noexcept;
        bool AABB(vec3 ext1, vec3 ext2, vec3 disp) noexcept;
        bool OBB(vec3 ext1, vec3 ext2, pose rel) noexcept;
        bool SphereCube(float r, vec3 ext, vec3 disp) noexcept;
    }
}