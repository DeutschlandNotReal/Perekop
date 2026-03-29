#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "geometry.hpp"
#include "glm/ext/matrix_float3x4.hpp"
#include "engine.hpp"
#include "glm/geometric.hpp"

using glm::vec3, glm::vec2, glm::mat3x4;

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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, 0);
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

    void MeshRenderer::draw(Camera cam) {
        vec2 screen_size = pk::engine::window::get_size();
        auto vp_matrix = cam.proj_matrix(screen_size.x, screen_size.y) * cam.view_matrix();
        glUseProgram(default_shader);
        glUniformMatrix4fv(
            glGetUniformLocation(default_shader, "VP"),
            1,
            GL_FALSE,
            (float*) &vp_matrix
        );

        ID_T& tcur = transforms_count, tsize = transforms_capacity;
        for (ID_T i = 0; i < meshes_count; i++) {
            Mesh& mesh = *(meshes[i]);
            glBindVertexArray(mesh.VAO);
            if (mesh.users_count > tsize) {
                delete[] transforms;
                tsize <<= 1;
                transforms = new mat3x4[tsize];
            }
            for (tcur = 0; tcur < mesh.users_count; tcur++) {
                Model* model = mesh.users[tcur];
                auto& mat = model->matrix;
                auto& scl = model->scl;
                auto& pos = model->pos;
                new (&transforms[tcur]) mat3x4{
                    {mat[0] * scl.x, pos.x},
                    {mat[1] * scl.y, pos.y},
                    {mat[2] * scl.z, pos.z}
                };
            }
            glBindBuffer(GL_ARRAY_BUFFER, mesh.IBO);
            glBufferData(GL_ARRAY_BUFFER, tcur * sizeof(glm::mat3x4), transforms, GL_DYNAMIC_DRAW);
            glDrawElementsInstanced(GL_TRIANGLES, mesh.triangle_count, GL_UNSIGNED_SHORT, 0, mesh.users_count);
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
        "#version 120"
        "\n attribute vec3 v;"
        "\n attribute vec4 t0;"
        "\n attribute vec4 t1;"
        "\n attribute vec4 t2;"
        "\n uniform mat4 VP;"
        "\n void main() {"
        "\n     mat4 model = mat4("
        "\n         vec4(t0.xyz, 0.0),"
        "\n         vec4(t1.xyz, 0.0),"
        "\n         vec4(t2.xyz, 0.0),"
        "\n         vec4(t0.w, t1.w, t2.w, 1.0)"
        "\n     );"
        "\n     gl_Position = VP * model * vec4(v, 1.0);"
        "\n };",

        "#version 120"
        "\n void main() {"
        "\n gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);"
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

    void Model::look_at(glm::vec3 at, glm::vec3 up) {
        auto look = glm::normalize(at - pos);
        auto right = glm::normalize(glm::cross(look, up));
        up = glm::normalize(up);

        matrix[0] = right;
        matrix[1] = up;
        matrix[2] = -look;
    }

    glm::mat4 Camera::view_matrix() const {
        return glm::lookAt(origin, look, vec3(0, 1, 0));
    };

    glm::mat4 Camera::proj_matrix(int x, int y) const {
        return glm::perspective(
            glm::radians(fov), 
            (float)x / (float)y,
            n, f
        );
    }
}