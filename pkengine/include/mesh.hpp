#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "camera.hpp"
#include <vector>
#include <functional>
#include <string>

#define u16 unsigned short
#define vec3 glm::vec3

namespace pk {
    class Model;
    class Mesh;

    namespace engine {
        void draw(std::vector<Mesh*>& mesh_pool, Camera camera);
    }
    class Mesh {
        friend Model; friend void engine::draw(std::vector<Mesh*>& mesh_pool, Camera camera);

        private:
            u16 tnum, vnum;
            unsigned int VAO, VBO, EBO, IBO;
            vec3* vert;
            u16* trig; 
            std::vector<Model*> users;
        public:
            Mesh() = delete;
            Mesh(u16 trig_count, u16 vert_count, u16* trigbuf, vec3* vertbuf); 
            ~Mesh();

            void set_position(u16 vid, vec3 pos) { if (vid < vnum) vert[vid] = pos; }

            void flush();
    };

    class Model {
        friend Mesh;
        private:
            u16 ref;
            Mesh* mesh;
        public:
            glm::mat3x3 matrix;
            vec3 pos = vec3(0), scl = vec3(1);
            void set_mesh(Mesh* M);
            Mesh* get_mesh() { return mesh; }
    };

    inline glm::mat4 Camera::view_matrix() const {
        return glm::lookAt(origin, look, vec3(0, 1, 0));
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
#undef vec3