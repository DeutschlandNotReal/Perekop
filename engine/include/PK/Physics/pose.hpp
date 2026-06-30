#pragma once
#define GLM_FORCE_INTRINSICS
#define GLM_FORCE_XYZW_ONLY
#include <PK/Core/number.hpp>
#include <PK/Core/simd.hpp>
#include <cmath>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

using glm::vec3, glm::quat, glm::mat4, glm::vec2, glm::mat3;

namespace pk {
    class Pose {
        // simd vec3 * quat
        static f32x4 rotate(f32x4 v, f32x4 q) noexcept {
            static constexpr f32 invmask = bit_cast<f32>(~0);
            static constexpr f32x4 xyz_mask{invmask, invmask, invmask, 0.f};

            f32x4 quat_w = q.swizzle<3, 3, 3, 3>();
            f32x4 quat_xyz = q & xyz_mask;

            return quat_xyz.cross(v.fmuladd(quat_w, quat_xyz.cross(v))).fmuladd(2.f, v);
        }

        static f32x4 conjugate(f32x4 q) noexcept {
            static constexpr f32 fmask = bit_cast<f32>(0x80000000); 
            static constexpr f32x4 mask = {fmask, fmask, fmask, 0.f};
            return q ^ mask;
        }

        public:
            // SIMD!! (w in pos unused, only xyz)
            f32x4 pos{0, 0, 0, 0}, rot{0, 0, 0, 1};
            
            operator vec3() const noexcept { return *(vec3*) &pos; }
            operator quat() const noexcept { return *(quat*) &rot; }

            Pose() = default;
            Pose(vec3 p): pos(&p.x) {}
            Pose(quat r): rot(&r.w) {}
            Pose(vec3 p, quat r):
                pos(&p.x),
                rot(&r.w)
            {}

            Pose(f32x4 p, f32x4 r): pos(p), rot(r) {}

            mat4 mat4() const noexcept {
                f32x4 dsqr = rot * rot * 2;

                return glm::translate(glm::mat4_cast(rot), pos);
            }

            Pose operator+(vec3 b) const noexcept { 
                return {pos + &b, rot};
            }

            Pose& operator+=(vec3 b) noexcept {
                pos += &b;
                return *this;
            }

            Pose operator-(vec3 b) const noexcept {
                return {pos - &b, rot};
            }

            Pose operator-=(vec3 b) noexcept {
                pos -= &b;
                return *this;
            }

            Pose operator*(Pose b) const {
                return {pos + rot * b.pos, glm::normalize(rot * b.rot)};
            }

            Pose operator*(quat q) const {
                return {pos, glm::normalize(rot * q)};
            }
                
            Pose& operator*=(quat q) {
                rot = glm::normalize(rot * q); return *this;
            }

            Pose& rotate(f32 angle, vec3 axis) {
                return *this *= glm::angleAxis(angle, glm::normalize(axis));
            }

            vec3 wspace_vec(vec3 v) const {
                return rot * v;
            }

            vec3 lspace_vec(vec3 v) const {
                return glm::conjugate(rot) * v;
            }

            vec3 wspace_point(vec3 p) const {
                return rot * p + pos;
            }

            vec3 lspace_point(vec3 p) const {
                return glm::conjugate(rot) * (p - pos);
            }

            vec3 fvec() const noexcept { 
                return rot * vec3{0, 0,-1};
            }

            vec3 rvec() const { 
                return rot * vec3{1, 0, 0}; 
            }
            vec3 uvec() const { 
                return rot * vec3{0, 1, 0};
            }

            Pose lerp(Pose b, f32 t) const {
                return {
                    glm::mix(pos, b.pos, t),
                    glm::slerp(rot, b.rot, t)
                };
            }

            quat delta_rot(Pose other) const {
                return glm::conjugate(rot) * other.rot;
            }

            vec3 delta_pos(Pose other) const {
                return lspace_point(other.pos);
            }
        
            void normalise() {
                rot = glm::normalize(rot);
            }
    };
}