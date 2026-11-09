#pragma once
#include <PK/Math/pose.hpp>
#include <PK/Core/vector.hpp>
#include <PK/Physics/body.hpp>
#include <PK/Core/set.hpp>
#include <PK/Physics/AABB.hpp>
#include <PK/Util/events.hpp>

namespace pk {
    class bodySeperator {
        struct identry {
            setID id; AABB ext;
        };

        array<identry> ids;

        struct node {
            AABB bound;
            ushort lhs, rhs, idlow, idhigh;
        };

        vector<node> nodes;

        public:
            void seperate(const set<body>& bodies);
            void traverse(AABB box, consumer<setID, vec3> checker);
    };
}