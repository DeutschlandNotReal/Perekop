#pragma once
#include <Perekop/Transform.hpp>
#include <Perekop/Mesh.hpp>

namespace pk {
    class Model {
        friend Mesh;
        ID_T ref = 0; // where this model is in its mesh's user
        Mesh* _mesh = nullptr;
        public:
            pk::Transform transform;
            glm::vec3 scl{1};
            void apply_mesh(Mesh* new_mesh);
            Mesh* mesh() const noexcept{ return _mesh; }
            Model() = default;
            Model(Mesh* M) { apply_mesh(M); }
            Model(Mesh* M, glm::vec3 p): transform(p) { apply_mesh(M); }

            Model(const Model&) = delete;
            Model& operator=(const Model&) = delete;
            ~Model();
    };

    struct Camera {
        float f = 200.f, n = .1f, fov = 75.f;
        pk::Transform transform;

        glm::mat4 get_VP(glm::vec2 screen_size) const noexcept;
    };
}