#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace pk {
    struct Pose {
        glm::vec3 pos{0};
        glm::quat rot{1, 0, 0, 0};

        operator glm::vec3() const { return pos; }
        operator glm::quat() const { return rot; }

        glm::mat4 mat4() const {
            return glm::translate(glm::identity<glm::mat4>(), pos) * glm::mat4_cast(rot);
        }

        Pose operator+(const glm::vec3& b) const { return {pos + b, rot}; }
        Pose& operator+=(const glm::vec3& b) { pos += b; return *this; }
        Pose operator-(const glm::vec3& b) const { return {pos - b, rot}; }
        Pose& operator-=(const glm::vec3& b) { pos -= b; return *this; }

        Pose operator*(const Pose& b) const {
            return {pos + rot * b.pos, glm::normalize(rot * b.rot)};
        }

        Pose operator*(const glm::quat& q) const {
            return {pos, glm::normalize(rot * q)};
        }
        
        Pose& operator*=(const glm::quat& q) {
            rot = glm::normalize(rot * q); return *this;
        }

        Pose& rotate(float angle_rad, glm::vec3 axis) {
            return *this *= glm::angleAxis(angle_rad, glm::normalize(axis));
        }

        glm::vec3 wspace_vec(glm::vec3 v) const {
            return rot * v;
        }

        glm::vec3 lspace_vec(glm::vec3 v) const {
            return glm::inverse(rot) * v;
        }

        glm::vec3 wspace_point(glm::vec3 p) const {
            return rot * p + pos;
        }

        glm::vec3 lspace_point(glm::vec3 p) const {
            return glm::inverse(rot) * (p - pos);
        }

        glm::vec3 fvec() const { return rot * glm::vec3{0, 0,-1}; }
        glm::vec3 rvec() const { return rot * glm::vec3{1, 0, 0}; }
        glm::vec3 uvec() const { return rot * glm::vec3{0, 1, 0}; }

        Pose lerp(const Pose& b, float t) {
            return {
                glm::mix(pos, b.pos, t),
                glm::slerp(rot, b.rot, t)
            };
        }

        glm::quat delta_rot(const Pose& other) const {
            return glm::inverse(rot) * other.rot;
        }
  
        void normalise() {
            rot = glm::normalize(rot);
        }
    };
}