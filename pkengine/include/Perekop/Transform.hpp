#pragma once
#include <glm/glm.hpp>

namespace pk {
    struct Transform {
        glm::vec3 pos{0};
        glm::mat3 rot{1};
        Transform() = default;
        Transform(const glm::mat3& rotation): rot(rotation) {}
        Transform(const glm::vec3& position): pos(position) {}
        Transform(const glm::vec3& position, glm::mat3 rotation): pos(position), rot(rotation) {}
        Transform(const glm::mat3x4& mat): 
            rot(glm::mat3{mat[0], mat[1], mat[2]}),
            pos(glm::vec3(mat[0].w, mat[1].w, mat[2].w)) {}
        Transform(const glm::mat4& mat): 
            rot(glm::mat3{mat[0], mat[1], mat[2]}), pos(glm::vec3(mat[3])) {}

        Transform operator*(const Transform& other) const noexcept { return {pos + rot * other.pos, rot * other.rot}; }
        Transform operator+(const Transform& other) const noexcept { return {pos + other.pos, rot}; }
        Transform operator-(const Transform& other) const noexcept { return {pos - other.pos, rot}; }

        operator glm::mat3() const { return rot; }
        operator glm::vec3() const { return pos; }
        operator glm::mat4() const {
            return {{rot[0], 0}, {rot[1], 0}, {rot[2], 0}, {pos, 1}};
        }

        operator glm::mat3x4() const {
            return {{rot[0], pos.x}, {rot[1], pos.y}, {rot[2], pos.z}};
        }

        void displace(const glm::vec3& delta) noexcept { pos += delta; }

        void displace_local(const glm::vec3& delta) noexcept { pos += rot * delta; }

        void normalize() noexcept {
            rot[0] = glm::normalize(rot[0]);
            rot[1] = glm::normalize(rot[1]);
            rot[2] = glm::normalize(rot[2]);
        }

        glm::mat3x4 scale(const glm::vec3& scale) const noexcept {
            return {{rot[0] * scale.x, pos.x}, {rot[1] * scale.y, pos.y}, {rot[2] * scale.z, pos.z}};
        }

        void rotateX(float theta) {
            float sx, cx;
            sincosf(theta, &sx, &cx);
            rot *= glm::mat3{
                1, 0, 0,
                0, cx, -sx,
                0, sx, cx
            };
        }

        void rotateY(float theta) {
            float sy, cy;
            sincosf(theta, &sy, &cy);
            rot *= glm::mat3{
                cy, 0, -sy,
                0, 1, 0,
                sy, 0, cy
            };
        }

        void rotateZ(float theta) {
            float sz, cz;
            sincosf(theta, &sz, &cz);
            rot *= glm::mat3{
                cz, sz, 0,
                -sz, cz, 0,
                0, 0, 1
            };
        }

        void rotateYXZ(float pitch = 0, float yaw = 0, float roll = 0) {
            float sx, sy, sz, cx, cy, cz;
            sincosf(pitch, &sx, &cx);
            sincosf(yaw, &sy, &cy);
            sincosf(roll, &sz, &cz);

            rot *= glm::mat3{
                cy * cz + sy * sx * sz, cy * sz + sy * sx * cz, sy * cx,
                cx * sz, cx * cz, -sx,
                -sy * cz + cy * sx * sz, sz * sy + cy * sx * cz ,cy * cx
            };
        }

        void setYXZ(float pitch = 0, float yaw = 0, float roll = 0) {
            float sx, sy, sz, cx, cy, cz;
            sincosf(pitch, &sx, &cx);
            sincosf(yaw, &sy, &cy);
            sincosf(roll, &sz, &cz);

            rot = glm::mat3{
                cy * cz + sy * sx * sz, cy * sz + sy * sx * cz, sy * cx,
                cx * sz, cx * cz, -sx,
                -sy * cz + cy * sx * sz, sz * sy + cy * sx * cz ,cy * cx
            };
        }
    };
}