#pragma once

#include <PK/Math/pose.hpp>
#include <PK/Core/set.hpp>
#include <PK/Render/Mesh.hpp>

namespace pk {
    class Model {
        id<Mesh> meshid;
        public:
            id<Mesh> getMesh() const noexcept { return meshid; }
            void setMesh(id<Mesh> Mesh) noexcept;

            pose pose;
            vec3 scale{1};
    };

    using ModelHandle = handle<Model>;
}