#pragma once
#include <PKLib/Math.hpp>

namespace pk {
    struct Pose {
        vec3 pos{0};
        quat rot{1, 0, 0, 0};

        operator vec3() const { return pos; }
        operator quat() const { return rot; }

        mat4 mat4() const {
            return glm::translate(glm::identity<glm::mat4>(), pos) * glm::mat4_cast(rot);
        }

        Pose operator+(const vec3& b) const { 
            return {pos + b, rot}; 
        }

        Pose& operator+=(const vec3& b) { 
            pos += b; return *this; 
        }

        Pose operator-(const vec3& b) const { 
            return {pos - b, rot}; 
        }

        Pose& operator-=(const vec3& b) { 
            pos -= b; return *this; 
        }

        Pose operator*(const Pose& b) const {
            return {pos + rot * b.pos, glm::normalize(rot * b.rot)};
        }

        Pose operator*(const quat& q) const {
            return {pos, glm::normalize(rot * q)};
        }
        
        Pose& operator*=(const quat& q) {
            rot = glm::normalize(rot * q); return *this;
        }

        Pose& rotate(float theta, vec3 axis) {
            return *this *= glm::angleAxis(theta, glm::normalize(axis));
        }

        vec3 wspace_vec(vec3 v) const {
            return rot * v;
        }

        vec3 lspace_vec(vec3 v) const {
            return glm::inverse(rot) * v;
        }

        vec3 wspace_point(vec3 p) const {
            return rot * p + pos;
        }

        vec3 lspace_point(vec3 p) const {
            return glm::inverse(rot) * (p - pos);
        }

        vec3 fvec() const { return rot * vec3{0, 0,-1}; }
        vec3 rvec() const { return rot * vec3{1, 0, 0}; }
        vec3 uvec() const { return rot * vec3{0, 1, 0}; }

        Pose lerp(const Pose& b, float t) {
            return {
                glm::mix(pos, b.pos, t),
                glm::slerp(rot, b.rot, t)
            };
        }

        quat delta_rot(const Pose& other) const {
            return glm::inverse(rot) * other.rot;
        }
  
        void normalise() {
            rot = glm::normalize(rot);
        }
    };
}