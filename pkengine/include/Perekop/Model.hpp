#pragma once
#include <Perekop/Transform.hpp>
#include <Perekop/Mesh.hpp>

namespace pk {
    class Model {
        friend Mesh;
        ID_T ref = 0; // where this model is in its mesh's user
        Mesh* mesh = nullptr;
        public:
            pk::Transform transform;
            glm::vec3 scl = glm::vec3(1);
            void set_mesh(Mesh* M);
            Mesh* get_mesh() { return mesh; }
            Model() = default;
            Model(Mesh* M) { set_mesh(M); }
            Model(Mesh* M, glm::vec3 p): transform(p) { set_mesh(M); }
            ~Model();
    };

    struct Camera {
        float f = 200, n = .1f;
        float fov = 75;
        pk::Transform transform;

        glm::mat4 get_viewproj(int screen_x, int screen_y) const;
    };
}