#pragma once
#include <PK/Physics/body.hpp>
#include <PK/Core/set.hpp>

namespace pk::Physics {
    struct CollisionGroup {
        vector<uint> modelIds;
        bool selfCollide{false};
    
        vector<uint> otherCollides{};
    };

    class SpatialSeperator {
        struct Entry {
            vec3 centre, ext;
            uint lhsid{0}, rhsid{0}, itemid{0};

            Entry(vec3 centre, vec3 ext, uint itemid = 0) noexcept:
                centre(centre), ext(ext), itemid(itemid) {}
        };

        struct Position {
            vec3 centre, ext;
            uint id;

            Position(vec3 centre, vec3 ext, uint id) noexcept:
            centre(centre), ext(ext), id(id) {}
        };

        vector<Entry> entries;
        vector<Position> positions;
        vector<uint> nearby;

        public:
            void seperate(const set<Body> &data);

            span<uint> near(vec3 O, vec3 ext) noexcept;
    };

    void Collide(Body& A, Body& B, float dt) noexcept;
}
