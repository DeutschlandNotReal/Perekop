#pragma once

#include <PK/Physics/body.hpp>
#include <PK/Core/set.hpp>
#include <PK/Math/pose.hpp>

namespace pk::physics {
    void solveJoints(float dt) noexcept;
    void correctJoints() noexcept;

    enum class jointType {
        spring, weld, hinge, angular, rope
    };

    class joint {
        public:
            jointType type;
            uint bodyid[2] = {0, 0};
            vec3 points[2] = {vec3{0}, vec3{0}};

            union {
                struct { float stiffness, length; }; // spring, rope
                struct { float maxangle; };          // angular
            };

            quat rel{1, 0, 0, 0}, rot0{1, 0, 0, 0};

            quat relRot{1.f, 0.f, 0.f, 0.f};
            quat rotA{1.f, 0.f, 0.f, 0.f};

            vec3 hingeAxes[2] = {vec3{0}, vec3{0}};

            static joint makeWeld() noexcept {
                return {
                    .type = jointType::weld
                };
            }

            static joint makeHinge(vec3 axisA, vec3 axisB) noexcept {
                return {
                    .type = jointType::hinge, 
                    .hingeAxes = {normalize(axisA), normalize(axisB)}
                };
            }

            static joint makeSpring(float stiffness, float length) noexcept {
                return {
                    .type = jointType::spring,
                    .stiffness = stiffness,
                    .length = length
                };
            }

            static joint makeAngular(float maxangle) noexcept {
                return {
                    .type = jointType::angular,
                    .maxangle = maxangle
                };
            }

            static joint makeRope(float length) noexcept {
                return {
                    .type = jointType::rope,
                    .length = length
                };
            }

            joint& attach0(entry<body> body, vec3 pos) noexcept {
                bodyid[0] = body.id;
                points[0] = pos;
                rotA = body.pose.rot;
                return *this;
            }

            joint& attach1(entry<body> body, vec3 pos) noexcept {
                bodyid[1] = body.id;
                points[1] = pos;
                if (type == jointType::weld)
                    relRot = conjugate(rotA) * body.pose.rot;
                
                return *this;
            }
    };
}