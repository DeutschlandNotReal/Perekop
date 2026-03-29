#pragma once
#include "glm/fwd.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define ID_T unsigned short

// shorthand but also makes resize function for each
#define PK_ARRAY(T, N) T* N; ID_T N##_count = 0; ID_T N##_capacity = 0; void resize_##N(ID_T size) { N##_capacity = size; T* prev = N; N = new T[size]; for (ID_T i = 0; i < N##_count; i++) N[i] = prev[i]; delete[] prev; };

namespace pk {
    class Model;
    class MeshRenderer;
    class Camera;

    class MeshVertex { glm::vec3 pos; glm::vec2 uv; };
    class MeshTriangle { ID_T v0, v1, v2; };

    class Mesh {
        friend Model; friend MeshRenderer;

        private:
            unsigned int VAO = 0, VBO = 0, EBO = 0, IBO = 0;
            ID_T meshid = 0 ;
            MeshRenderer* renderer;
            
            PK_ARRAY(MeshVertex, vertex)
            PK_ARRAY(MeshTriangle, triangle)
            PK_ARRAY(Model*, users)

            ~Mesh();
        public:
            ID_T push_vertex(MeshVertex vertex);
            ID_T pop_vertex();
            void set_vertex(ID_T vid, MeshVertex new_vertex);

            ID_T push_triangle(MeshTriangle triangle);
            ID_T pop_triangle();
            void set_triangle(ID_T vid, MeshTriangle new_triangle);

            void load(); // into VRAM
            void flush();
            void unload(); // outta VRAM

            void dismiss();
    };

    class MeshRenderer {
        friend Mesh;
        private:
            PK_ARRAY(Mesh*, meshes)
            PK_ARRAY(glm::mat3x4, transforms)
        public:
            Mesh* create_mesh();
            void draw(Camera cam);    
            ~MeshRenderer(); 
    };

    class Model {
        friend Mesh;
        private:
            ID_T ref = 0; // where this model is in its mesh's user
            Mesh* mesh = nullptr;
        public:
            glm::mat3x3 matrix = glm::mat3x3(1);
            glm::vec3 pos = glm::vec3(0), scl = glm::vec3(1);
            void set_mesh(Mesh* M);
            Mesh* get_mesh() { return mesh; }
            ~Model();
    };

    struct Camera {
        float f = 100, n = .01f;
        float fov = 75;
        glm::vec3 origin;
        glm::vec3 look;

        glm::mat4 view_matrix() const;
        glm::mat4 proj_matrix(int x, int y) const;
    };
}

#undef PK_ARRAY