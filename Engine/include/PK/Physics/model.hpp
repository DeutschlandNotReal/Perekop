#pragma once

#include <PK/Math/pose.hpp>
#include <PK/Core/set.hpp>
#include <PK/Render/Mesh.hpp>

namespace pk {
    class Model {
        public:
            MeshID meshID;
           
            pose pose;
            vec3 scale{1};
            AABB bounds;

            AABB getWorldAABB() const noexcept {
                mat3 R = (mat3) pose;

                vec3 ext    = abs(R) * (abs(scale) * bounds.getExtents());
                vec3 centre = pose.pos + R * (scale * bounds.getCentre());

                return AABB::fromExtents(centre, ext);
            }
    };

    using ModelHandle = handle<Model>;
    using ModelID     = SID<Model>;
}