#include "gl.hpp"
#include "mesh.hpp"
#include "glm/ext/matrix_float3x4.hpp"
#include "engine.hpp"

using glm::vec3, glm::vec2;
#define u16 unsigned short

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
    static unsigned int default_shader = load_program(
        "\n #version 120"
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

        "\n#version 120"
        "\n void main() {"
        "\n gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);"
        "\n };"
    );

    void Mesh::flush() {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, V.size() * 12, V.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, T.size() * 2, T.data(), GL_STATIC_DRAW);
    }

    Mesh::Mesh(u16 i): ref(i) {
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

        // each row of the transforms need its own attrib array, opengl just sloppy like that
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

    Mesh::~Mesh() {
        for (Model* m : users) { m->mesh = nullptr; }
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &IBO);
        glDeleteVertexArrays(1, &VAO);
    }

    Mesh* MeshRenderer::new_mesh() {
        Mesh* mesh = new Mesh(meshes.size());
        meshes.push_back(mesh);

        return mesh;
    };

    void MeshRenderer::draw() {
        auto VP = cam.proj_matrix(pk::engine::window::get_size()) * cam.view_matrix();
        glUseProgram(default_shader);
        glUniformMatrix4fv(
            glGetUniformLocation(default_shader, "VP"), 
            1, 
            GL_FALSE, 
            &VP[0][0]
        );
        for (Mesh* mesh : meshes) {
            glBindVertexArray(mesh->VAO);
            glm::mat3x4* transforms = new glm::mat3x4[mesh->users.size()];
            for (int i = 0; i < mesh->users.size(); i++) {
                Model* model = mesh->users[i];
                auto& mat = model->matrix;
                auto& scl = model->scale;
                auto& pos = model->pos;

                transforms[i] = glm::mat3x4 {
                    {mat[0] * scl.x, pos.x},
                    {mat[1] * scl.y, pos.y},
                    {mat[2] * scl.z, pos.z},
                };
            }

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->IBO);
            glBufferData(GL_ARRAY_BUFFER, mesh->users.size() * sizeof(glm::mat3x4), transforms, GL_DYNAMIC_DRAW);
            glDrawElementsInstanced(GL_TRIANGLES, mesh->T.size(), GL_UNSIGNED_SHORT, 0, mesh->users.size());

            delete[] transforms;
        }
        glBindVertexArray(0);
    }

    void Model::set_mesh(Mesh* M) {
       if (M != mesh) {
        if (mesh != nullptr) {
            auto& users = mesh->users;
            users.back()->ref = ref;
            users[ref] = users.back();
            users.pop_back();
        };
        mesh = M;
        if (mesh == nullptr) return;

        ref = mesh->users.size();
        mesh->users.push_back(this);
       }
    }

    u16 Mesh::new_vert(vec3 p) { V.emplace_back(p); return V.size() - 1; }
    u16 Mesh::pop_vert() { V.pop_back(); return V.size(); }
    void Mesh::set_pos(u16 vid, vec3 p) { V[vid] = p; }

    // BLOAT (debloat soon)
    u16 Mesh::new_trig(u16 v0, u16 v1, u16 v2) { T.push_back(v0); T.push_back(v1); T.push_back(v2); return T.size() / 3 - 1; }
    u16 Mesh::pop_trig() { T.pop_back(); T.pop_back(); T.pop_back(); return T.size() / 3; }
    void Mesh::set_trig(u16 tid, u16 v0, u16 v1, u16 v2) {
        T[tid * 3] = v0;
        T[tid * 3 + 1] = v1;
        T[tid * 3 + 2] = v2;
    }

}