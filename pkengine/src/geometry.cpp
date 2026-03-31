#include "glm/matrix.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <Perekop/Geometry.hpp>
#include <Perekop/Engine.hpp>

using namespace glm;
using namespace Perekop;

GLuint load_shader(const char* src, GLenum type) {
    // should probably make it detect sloppy shader code
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    return shader;
}

GLuint load_program(const char* vsrc, const char* fsrc) {
    GLuint vshader = load_shader(vsrc, GL_VERTEX_SHADER);
    GLuint fshader = load_shader(fsrc, GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
        
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    glLinkProgram(program);
    return program;
}

namespace pk {
    static unsigned int default_shader;

    void Mesh::flush() {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(MeshVertex), vertex, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangle_count * sizeof(MeshTriangle), triangle, GL_STATIC_DRAW);
    }

    void Mesh::load() {
        if (VAO) return;
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &IBO);
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBindBuffer(GL_ARRAY_BUFFER, IBO);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat3x4), (void*)0);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat3x4), (void*)16);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat3x4), (void*)32);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);

        glVertexAttribDivisor(1, 1);
        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);

        glBindVertexArray(0);
    }

    void Mesh::unload() {
        if (!VAO) return;
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &IBO);
        glDeleteVertexArrays(1, &VAO);
        VAO = 0, VBO = 0, EBO = 0, IBO = 0;
    }

    void Mesh::dismiss() {
        unload();
        if (!renderer) return;
        auto& meshes = renderer->meshes;
        meshes[meshid] = meshes[--renderer->meshes_count];
        meshes[meshid]->meshid = meshid;
        renderer = nullptr;
    }
    
    ID_T Mesh::push_vertex(MeshVertex vert) {
        if (vertex_capacity == vertex_count) { resize_vertex(vertex_capacity << 1); }
        vertex[vertex_count] = vert;
        return vertex_count++;
    }

    ID_T Mesh::pop_vertex() {
        if (!vertex_count) return 0;
        return --vertex_count;
    }

    void Mesh::set_vertex(ID_T vid, MeshVertex vert) {
        if (vid >= vertex_count) return;
        vertex[vid] = vert;
    }

    ID_T Mesh::push_triangle(MeshTriangle trig) {
        if (triangle_capacity == triangle_count) { resize_triangle(triangle_capacity << 1); }
        triangle[triangle_count] = trig;
        return triangle_count++;
    }

    ID_T Mesh::pop_triangle() {
        if (!triangle_count) return 0;
        return --triangle_count;
    }

    void Mesh::set_triangle(ID_T tid, MeshTriangle trig) {
        if (tid >= triangle_count) return;
        triangle[tid] = trig;
    }

    Mesh::~Mesh() {
        for (int i = 0; i < users_count; i++) { users[i]->mesh = nullptr; }
        unload();
        delete[] users; delete[] vertex; delete[] triangle;
    }

    void MeshRenderer::draw() {
        int screenx, screeny;
        Perekop::Window::get_size(screenx, screeny);
        auto VP = Perekop::Scene::camera.get_viewproj(screenx, screeny);
        glUseProgram(default_shader);
        glUniformMatrix4fv(
            glGetUniformLocation(default_shader, "VP"),
            1,
            GL_FALSE,
            (float*) &VP
        );

        int drawn_meshes = 0, drawn_models = 0;

        ID_T& tcur = transforms_count, tsize = transforms_capacity;
        for (ID_T i = 0; i < meshes_count; i++) {
            Mesh& mesh = *(meshes[i]);
            glBindVertexArray(mesh.VAO);
            if (mesh.users_count > tsize) {
                delete[] transforms;
                tsize = mesh.users_count;
                transforms = new mat3x4[tsize];
            }
            drawn_meshes++;
            drawn_models+=mesh.users_count;
            for (tcur = 0; tcur < mesh.users_count; tcur++) {
                Model* model = mesh.users[tcur];
                new (&transforms[tcur]) mat3x4{
                    model->transform.scale(model->scl)
                };
            }
            glBindBuffer(GL_ARRAY_BUFFER, mesh.IBO);
            glBufferData(GL_ARRAY_BUFFER, tcur * sizeof(glm::mat3x4), transforms, GL_DYNAMIC_DRAW);
            glDrawElementsInstanced(GL_TRIANGLES, mesh.triangle_count * 3, GL_UNSIGNED_SHORT, 0, mesh.users_count);
            glBindVertexArray(0);
        }
    }

    Mesh* MeshRenderer::create_mesh() {
        Mesh* mesh = new Mesh();
        if (meshes_count == meshes_capacity) { resize_meshes(meshes_capacity << 1); }
        meshes[meshes_count++] = mesh;
        return mesh;
    }

    MeshRenderer::~MeshRenderer() { 
        delete[] meshes; delete[] transforms; 
        for (ID_T i = 0; i < meshes_count; i++) {
            meshes[i]->renderer = nullptr;
        }
    }

    void MeshRenderer::init() {
        default_shader = load_program(
            // maybe writing shaders like this isnt the meta
        "#version 330"
        "\n in vec3 v;"
        "\n in vec4 t0;"
        "\n in vec4 t1;"
        "\n in vec4 t2;"
        "\n out float Z;"
        "\n uniform mat4 VP;"
        "\n void main() {"
        "\n     mat4 model = mat4("
        "\n         vec4(t0.xyz, 0.0),"
        "\n         vec4(t1.xyz, 0.0),"
        "\n         vec4(t2.xyz, 0.0),"
        "\n         vec4(t0.w, t1.w, t2.w, 1.0)"
        "\n     );"
        "\n     vec4 clip = VP * model * vec4(v, 1.0);"
        "\n     Z = clip.z / clip.w;"
        "\n     Z = Z * 0.5 + 0.5;"
        "\n     gl_Position = clip;"
        "\n };",

        "#version 330"
        "\n in float Z;"
        "\n out vec4 fragColor;"
        "\n void main() {"
        "\n     fragColor = vec4(1.0 / (1+Z), 0.0, 0.0, 1.0);"
        "\n };"
    );
    }

    void Model::set_mesh(Mesh* M) {
       if (M == mesh) return;
       if (mesh) { // already has mesh (disconnect)
            mesh->users[ref] = mesh->users[--mesh->users_count];
       }
       mesh = M;
       if (!mesh) return;
       auto& count = mesh->users_count;
       auto& cap = mesh->users_capacity;
       ref = count;
       if (count == cap) { mesh->resize_users(cap << 1); }
       mesh->users[count++] = this;
    }

    Model::~Model() { set_mesh(nullptr); }

    glm::mat4 Camera::get_viewproj(int screen_x, int screen_y) const {
        return glm::perspective(
            glm::radians(fov), 
            (float)screen_x / (float)screen_y,
            n, f
        ) * glm::inverse((mat4)transform);
    }
}