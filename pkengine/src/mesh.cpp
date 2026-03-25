#include "gl.hpp"
#include "mesh.hpp"
#include "glm/ext/matrix_float3x4.hpp"
#include "util.hpp"

using glm::vec3, glm::vec2;
using uint = unsigned int;

uint load_shader(const char* src, GLenum type) {
    // should probably make it detect sloppy shader code
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    return shader;
}

uint load_program(const char* vsrc, const char* fsrc) {
    uint vshader = load_shader(vsrc, GL_VERTEX_SHADER);
    uint fshader = load_shader(fsrc, GL_FRAGMENT_SHADER);

    uint program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
        
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    glLinkProgram(program);
    return program;
}

namespace pk {
    static uint default_shader = load_program(
        "#version 120\n"
        "attribute vec4 in_vertex;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main() {\n"
        "   gl_Position = projection * view * model * in_vertex;\n"
        "}\n",

        "#version 120\n"
        "void main() {\n"
        "gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "};\n"
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
        glUseProgram(default_shader);
        for (Mesh* mesh : meshes) {
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
            glBindBuffer(GL_ARRAY_BUFFER, mesh->IBO);
            glBufferData(GL_ARRAY_BUFFER, mesh->users.size() * sizeof(glm::mat3x4), transforms, GL_STATIC_DRAW);

            delete[] transforms;
        }
        glBindVertexArray(0);
    }

    void Model::set_mesh(Mesh* M) {
       if (M != mesh) {
        util::swappop<Model*>(mesh->users, ref, [this](auto& V){ V->ref = ref; });
        mesh = M;
        if (mesh == nullptr) return;

        ref = mesh->users.size();
        mesh->users.push_back(this);
       }
    }


}