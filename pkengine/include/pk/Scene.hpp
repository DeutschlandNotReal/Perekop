#pragma once
#include <pk/Mesh.hpp>
#include <pk/Model.hpp>
#include <pk/glAttribute.hpp>

namespace pk {
    class Scene {
        Array<glm::mat3x4> transforms{50};

        public:
            SparseSet<Mesh> meshes;
            SparseSet<Model> models;
            
            void link(uint meshid, uint modelid) {
                meshes[meshid].models.push(modelid);
            }

            void draw(const Window& win);
    };
}