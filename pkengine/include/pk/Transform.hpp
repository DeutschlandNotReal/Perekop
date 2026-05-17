#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace pk {
    struct Transform {
        glm::vec3 pos{0};
        glm::mat3 rot{1};
        Transform() = default;
        Transform(const glm::mat3& rotation): 
            rot(rotation) 
        {}
        Transform(const glm::vec3& position): 
            pos(position) 
        {}
        Transform(const glm::vec3& position, glm::mat3 rotation): 
            pos(position), 
            rot(rotation) 
        {}
        Transform(const glm::mat3x4& mat): 
            rot(mat[0], mat[1], mat[2]),
            pos(mat[0].w, mat[1].w, mat[2].w) 
        {}
        Transform(const glm::mat4& mat): 
            rot(mat[0], mat[1], mat[2]), pos(mat[3]) 
        {}

        Transform operator*(const Transform& b) const noexcept { return {pos + rot * b.pos, rot * b.rot}; }
        Transform operator+(const Transform& b) const noexcept { return {pos + b.pos, rot}; }
        Transform operator-(const Transform& b) const noexcept { return {pos - b.pos, rot}; }
        bool operator==(const Transform& b) const noexcept { return pos==b.pos && rot==b.rot; }
        bool operator==(const glm::vec3& b) const noexcept { return pos== b; }
        bool operator==(const glm::mat3& b) const noexcept { return rot== b; }
        Transform& operator=(const glm::vec3 b) noexcept { pos = b; return *this; }

        operator glm::mat3() const noexcept { return rot; }
        operator glm::vec3() const noexcept { return pos; }
        operator glm::mat4() const noexcept {
            return {{rot[0], 0}, {rot[1], 0}, {rot[2], 0}, {pos, 1}};
        }

        operator glm::mat3x4() const noexcept {
            return {{rot[0], pos.x}, {rot[1], pos.y}, {rot[2], pos.z}};
        }

        void displace(const glm::vec3& disp) noexcept { pos += disp; }
        void displace_l(const glm::vec3& disp) noexcept { pos += rot * disp; }

        void orthogonize() noexcept {
            rot[0] /= glm::length(rot[0]);
            rot[1] = glm::normalize(glm::cross(rot[0], {0, 1, 0}));
            rot[2] = glm::normalize(glm::cross(rot[0], rot[1]));
        }

        void rotate_axis(glm::vec3 axis, float angle) noexcept {
            rot = glm::mat3{glm::rotate(glm::mat4{1}, angle, axis)} * rot;
        }

        void rotate_YXZ(float dX = 0, float dY = 0, float dZ = 0) {
            rot = glm::mat3{
                glm::rotate(glm::mat4{1}, dY, {0, 1, 0}) *
                glm::rotate(glm::mat4{1}, dX, rot[0]) *
                glm::rotate(glm::mat4{1}, dZ, rot[2])
            } * rot;
        }
    };
}