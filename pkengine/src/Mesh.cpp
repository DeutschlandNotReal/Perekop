#include "Perekop/Transform.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <Perekop/Engine.hpp>
#include <Perekop/Mesh.hpp>

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
    glLinkProgram(program);
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    return program;
}

struct glAttribute {
    long long offset{0}, stride{0}, index{0}, divisor{-1};
    GLenum last_buffer;
    glAttribute(GLuint array) { glBindVertexArray(array); }
    glAttribute(GLuint* array) {
        glGenVertexArrays(1, array);
        glBindVertexArray(*array);
    }
    template <int Q> inline glAttribute& genbuf(GLuint* buffer) {
        glGenBuffers(Q, buffer);
        return *this;
    }
    template <typename T> inline glAttribute& object() {
        stride = sizeof(T); offset = 0;
        return *this;
    }
    template <int K, GLenum T> inline glAttribute& member() {
        glVertexAttribPointer(index, K, T, GL_FALSE, stride, (void*)offset);
        glEnableVertexAttribArray(index);
        glVertexAttribDivisor(index, divisor);
        offset += K * sizeof(float); 
        ++index;
        return *this;
    }
    template <GLenum type> inline glAttribute& bind(GLuint buffer) { 
        if constexpr (type == GL_ARRAY_BUFFER) ++divisor;
        last_buffer = buffer;
        glBindBuffer(type, buffer); 
        return *this; 
    }
    template <GLenum mode, typename T> inline glAttribute& data(const pk::Array<T>& content) {
        glBufferData(last_buffer, content.bytesize(), content.data(), mode);
        return *this;
    }

    template <GLenum type> inline glAttribute& draw(int n_trig, int n_users) {
        glDrawElementsInstanced(GL_TRIANGLES, n_trig * 3, type, 0, n_users);
        return *this;
    }

    inline void end() { glBindVertexArray(0); }
};

namespace pk {
    glm::mat4 Camera::VP(glm::vec2 screen_size) const noexcept {
        return glm::perspective(
            glm::radians(fov), 
            screen_size.x / screen_size.y,
            n, f
        ) * glm::inverse((glm::mat4)transform);
    }

    MeshMaterial::MeshMaterial(const char* vsrc, const char* fsrc) {
        program = load_program(vsrc, fsrc);
    }

    void Mesh::bounds(glm::vec3& min, glm::vec3& max) const noexcept {
        if (!vertex.empty()) {
            min = vertex[0].pos, max = vertex[0].pos;
            for (int i = 1; i < vertex.size(); i++) {
                auto p = vertex[i].pos;
                min = glm::min(min, p);
                max = glm::max(max, p);
            }
        } else {
            min = max = glm::vec3{0};
        }
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

        glAttribute(VAO)
            .bind<GL_ARRAY_BUFFER>(VBO)
                .data<GL_STATIC_DRAW>(vertex)
            .bind<GL_ELEMENT_ARRAY_BUFFER>(EBO)
                .data<GL_STATIC_DRAW>(triangle)
        .end();
    }

    void Mesh::load() {
        if (VAO) return;
        glAttribute(&VAO)
            .genbuf<3>(&VBO)
            .bind<GL_ELEMENT_ARRAY_BUFFER>(EBO)
            .bind<GL_ARRAY_BUFFER>(VBO)
            .object<MeshVertex>()
                .member<3, GL_FLOAT>() // pos
                .member<2, GL_FLOAT>() // uv

            .bind<GL_ARRAY_BUFFER>(IBO)
            .object<mat3x4>()
                .member<4, GL_FLOAT>()
                .member<4, GL_FLOAT>()
                .member<4, GL_FLOAT>()
        .end();
    }

    void Mesh::unload() {
        if (!VAO) return;
        glDeleteBuffers(3, &VBO);
        glDeleteVertexArrays(1, &VAO);
        VAO = VBO = EBO = IBO = 0;
    }

    Mesh::~Mesh() { unload(); }

    void Scene::draw(const Window& window) {
        window.make_context();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        const mat4 VP = camera.VP(window.size());

        for (Mesh* mesh : meshes) {
            transforms.clear();
            transforms.reserve(mesh->users.size());

            for (Model* user : mesh->users)
                transforms.rawpush(user->transform.scale(user->scl));

            glAttribute(mesh->VAO)
                .bind<GL_ARRAY_BUFFER>(mesh->IBO)
                .data<GL_DYNAMIC_DRAW>(transforms)
                .draw<GL_UNSIGNED_SHORT>(mesh->triangle.size(), transforms.size())
            .end();
        }
    }

    
}