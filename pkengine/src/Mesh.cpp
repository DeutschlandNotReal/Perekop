#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <Perekop/Engine.hpp>
#include <Perekop/Mesh.hpp>
#include <Perekop/Model.hpp>
#include <iostream>

using namespace glm;
using namespace Perekop;

const char* pre_vsrc = "#version 330"
    "\n layout(location = 0) in vec3 _pos;"
    "\n layout(location = 1) in vec2 _uv;"
    "\n layout(location = 2) in vec4 M0;"
    "\n layout(location = 3) in vec4 M1;"
    "\n layout(location = 4) in vec4 M2;"
    "\n uniform mat4 VP;"
    "\n mat4 model() {"
    "\n     return mat4(vec4(M0.xyz, 0.0), vec4(M1.xyz, 0.0), vec4(M2.xyz, 0.0), vec4(M0.w, M1.w, M2.w, 1.0)); "
    "\n }";

const char* pre_fsrc = "#version 330 \n out vec4 fragColor;";

GLuint load_shader(const char** src, GLenum type) {
    // should probably make it detect sloppy shader code
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 2, src, NULL);
    glCompileShader(shader);
    return shader;
}

GLuint load_program(const char* vsrc, const char* fsrc) {
    const char* full_vsrc[] = {pre_vsrc, vsrc};
    const char* full_fsrc[] = {pre_fsrc, fsrc};
    GLuint vshader = load_shader(full_vsrc, GL_VERTEX_SHADER);
    GLuint fshader = load_shader(full_fsrc, GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
        
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    glLinkProgram(program);
    return program;
}

struct attrib_builder {
    size_t offset = 0, stride = 0;
    GLuint index = 0, divisor = 0;

    template <typename T> inline attrib_builder& object(int div = 0) {
        stride = sizeof(T); offset = 0; divisor = div;
        return *this;
    }

    template <int K, GLenum T> inline attrib_builder& member() {
        glVertexAttribPointer(index, K, T, GL_FALSE, stride, (void*)offset);
        glEnableVertexAttribArray(index);
        glVertexAttribDivisor(index, divisor);
        offset += K * sizeof(float); 
        ++index;
        return *this;
    }

    template <GLenum type> inline attrib_builder& bind(GLuint buffer) { glBindBuffer(type, buffer); return *this; }
    

    inline void end() { glBindVertexArray(0); }
};

namespace pk {
    MeshMaterial::MeshMaterial(const char* vsrc, const char* fsrc) {
        program = load_program(vsrc, fsrc);
    }

    MeshBounds Mesh::bounds() const noexcept {
        if (vertex.empty()) return MeshBounds();
        glm::vec3 min = vertex[1].pos, max = vertex[1].pos;

        for (const MeshVertex& v : vertex) {
            min = glm::min(min, v.pos);
            max = glm::max(max, v.pos);
        }
        return {min, max};
    }

    void MeshMaterial::enable(const mat4& VP) {
        glUseProgram(program);
        glUniformMatrix4fv(
            glGetUniformLocation(program, "VP"),
            1,
            GL_FALSE,
            (float*) &VP
        );
    }

    void Mesh::refresh() {
        if (!VAO) load();
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,  vertex.bytes(), vertex.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangle.bytes(), triangle.data(), GL_STATIC_DRAW);
    }

    void Mesh::load() {
        if (VAO) return;
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &IBO);
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        attrib_builder()
            .bind<GL_ARRAY_BUFFER>(VBO)
            .object<MeshVertex>()
                .member<3, GL_FLOAT>() // pos
                .member<2, GL_FLOAT>() // uv
            .bind<GL_ARRAY_BUFFER>(IBO)
            .bind<GL_ELEMENT_ARRAY_BUFFER>(EBO)
            .object<mat3x4>(1)
                .member<4, GL_FLOAT>()
                .member<4, GL_FLOAT>()
                .member<4, GL_FLOAT>()
        .end();
    }

    void Mesh::unload() {
        if (!VAO) return;
        glDeleteBuffers(1,   &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &IBO);
        glDeleteVertexArrays(1, &VAO);
        VAO = 0, VBO = 0, EBO = 0, IBO = 0;
    }

    Mesh::~Mesh() {
        for (Model* model : users ) model->mesh = nullptr;
        unload();
        if (!renderer) return;
        renderer->available.push(meshid);
    }

    void MeshRenderer::draw() {
        const mat4 VP = Perekop::Scene::camera.get_viewproj(Window::size());
        for (ID_T i = 0; i < meshes.size(); i++) {
            Mesh& mesh = meshes[i];
            if (mesh.users.empty() || !mesh.material.program) continue;
            transforms.clear();
            mesh.material.enable(VP);
            glBindVertexArray(mesh.VAO);

            for (Model* model : mesh.users)
                transforms.push(model->transform.scale(model->scl));

            glBindBuffer(GL_ARRAY_BUFFER, mesh.IBO);
            glBufferData(GL_ARRAY_BUFFER, transforms.bytes(), transforms.data(), GL_DYNAMIC_DRAW);
            glDrawElementsInstanced(GL_TRIANGLES, mesh.triangle.size() * 3, GL_UNSIGNED_SHORT, 0, mesh.users.size());
        }

        glBindVertexArray(0);
    }

    MeshRenderer::~MeshRenderer() { 
        for (ID_T i = 0; i < meshes.size(); i++)
            meshes[i].renderer = nullptr;
    }

    Mesh& MeshRenderer::create(MeshMaterial material) {
        if (available.empty()) {
            ID_T i = available.pop();
            meshes[i] = Mesh(material);
            meshes[i].meshid = i;
            return meshes[i];
        }
        
        Mesh& mesh = meshes.emplace(material);
        mesh.meshid = meshes.size() - 1;
        return mesh;
    }
}