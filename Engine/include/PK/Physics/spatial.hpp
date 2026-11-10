#pragma once
#include <PK/Math/pose.hpp>
#include <PK/Core/vector.hpp>
#include <PK/Physics/Body.hpp>
#include <PK/Core/set.hpp>
#include <PK/Physics/AABB.hpp>
#include <PK/Util/events.hpp>

namespace pk {
    class bodySeperator {
        struct identry {
            uint id; AABB ext;
        };

        array<identry> ids;

        struct node {
            AABB bound;
            ushort lhs, rhs, idlow, idhigh;
        };

        vector<node> nodes;

        public:
            void seperate(const set<Body>& bodies);
            void traverse(AABB box, consumer<uint, vec3> checker);
    };
}