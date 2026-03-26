#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "camera.hpp"
#include <vector>
#include <functional>
#include <string>

#define u16 unsigned short
#define v3 glm::vec3

namespace pk {
    class Model;
    class MeshRenderer;

    class Mesh {
        friend Model; friend MeshRenderer;

        private:
            u16 ref;
            unsigned int VAO, VBO, EBO, IBO;
            std::vector<v3> V;
            std::vector<u16> T;
            std::vector<Model*> users;
            Mesh(u16 i);
        public:
            Mesh() = delete;
            ~Mesh();

            inline u16 new_trig(u16 v0, u16 v1, u16 v2);
            inline u16 pop_trig();
            inline void set_trig(u16 tid, u16 v0, u16 v1, u16 v2);

            inline u16 new_vert(v3 p);
            inline u16 pop_vert();
            inline void set_pos(u16 vid, v3 p);

            void flush();
    };

    class Model {
        friend Mesh;

        private:
            u16 ref;
            Mesh* mesh;
        public:
            glm::mat3x3 matrix;
            v3 pos;
            v3 scale;
            void set_mesh(Mesh* M);
            Mesh* get_mesh() { return mesh; }
    };

    class MeshRenderer {
        private:
            std::vector<Mesh*> meshes;
        public:
            int mesh_count() { return meshes.size(); }
            void draw();
            Mesh* new_mesh();
            Camera cam;
    };

    inline glm::mat4 Camera::view_matrix() const {
        return glm::lookAt(origin, look, glm::vec3(0, 1, 0));
    }

    inline glm::mat4 Camera::proj_matrix(glm::vec2 dim) const {
        return glm::perspective(
            glm::radians(fov), 
            (float)dim.x / (float)dim.y,
            n, f
        );
    }
}

#undef u16
#undef v3