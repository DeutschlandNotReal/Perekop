#include "glm/ext/matrix_clip_space.hpp"
#include "pk/Time.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <pk/Engine.hpp>
#include <pk/Mesh.hpp>

using namespace glm;

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

template <GLenum T> GLuint load_shader(const char* pre, const char* post) {
    const char* src[] = {pre, post};
    GLuint shader = glCreateShader(T);
    glShaderSource(shader, 2, src, NULL);
    glCompileShader(shader);
    return shader;
}

GLuint load_program(const char* vsrc, const char* fsrc) {
    GLuint 
        vshader = load_shader<GL_VERTEX_SHADER>(pre_vsrc, vsrc),
        fshader = load_shader<GL_FRAGMENT_SHADER>(pre_fsrc, fsrc),
        program = glCreateProgram();

    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    glLinkProgram(program);
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    return program;
}

struct glAttribute {
    long long i{0}, d{-1}, s{0}, o{0};
    GLuint b;
    glAttribute(GLuint array) { 
        glBindVertexArray(array); 
    }
    glAttribute(GLuint* array) {
        glGenVertexArrays(1, array);
        glBindVertexArray(*array);
    }
    template <typename T> glAttribute& object() {
        s = sizeof(T); o = 0; return *this;
    }
    template <typename T> glAttribute& fmember() {
        char kfloats = sizeof(T) >> 2;
        glVertexAttribPointer(i, kfloats, GL_FLOAT, GL_FALSE, s, (void*)o);
        glEnableVertexAttribArray(i);
        glVertexAttribDivisor(i, d);
        o += sizeof(T); ++i;
        return *this;
    }
    template <GLenum type> inline glAttribute& bind(GLuint buffer) { 
        if constexpr (type == GL_ARRAY_BUFFER) ++d;
        glBindBuffer(type, b = buffer); 
        return *this; 
    }
    template <GLenum type> inline glAttribute& bind(GLuint* buffer) {
        glGenBuffers(1, buffer);
        return bind<type>(*buffer);
    }
    template <GLenum mode, typename T> inline glAttribute& data(const pkutil::Array<T>& content) {
        glBufferData(b, content.size() * sizeof(T), content.begin(), mode);
        return *this;
    }
    template <GLenum type> inline glAttribute& draw(int n_ind, int n_users) {
        glDrawElementsInstanced(GL_TRIANGLES, n_ind, type, 0, n_users);
        return *this;
    }
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
        if (vertex.size() > 0) {
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

    void Mesh::rewind() noexcept {
        for (int i = 0; i < indices.size(); i+=3) {
            short first = indices[i];
            indices[i] = indices[i+2];
            indices[i+2] = first;
        }
    }

    void MeshMaterial::enable(const mat4& VP) const {
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
                .data<GL_STATIC_DRAW>(indices)
        ;
    }

    void Mesh::load() {
        if (VAO) return;
        glAttribute(&VAO)
            .bind<GL_ELEMENT_ARRAY_BUFFER>(&EBO)
            .bind<GL_ARRAY_BUFFER>(&VBO)
            .object<MeshVertex>()
                .fmember<glm::vec3>() // pos
                .fmember<glm::vec2>() // uv
            .bind<GL_ARRAY_BUFFER>(&IBO)
            .object<glm::mat3x4>()
                .fmember<glm::vec4>()
                .fmember<glm::vec4>()
                .fmember<glm::vec4>()
        ;
    }

    void Mesh::unload() {
        if (!VAO) return;
        glDeleteBuffers(3, &VBO);
        glDeleteVertexArrays(1, &VAO);
        VAO = VBO = EBO = IBO = 0;
    }

    void Scene::draw(const Window& window) {
        window.set_context();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        const mat4 VP = Perekop::camera.VP(window.size());

        StackTimer<double, 1> drawtimer;
        drawtimer.begin();
        int kmesh{0}, kmodel{0};
        for (const Mesh& mesh : meshes) {
            if (mesh.models.size() == 0) continue;
            ++kmesh;
            kmodel += mesh.models.size();
            transforms.clear();
            int required = mesh.models.size() - transforms.capacity();
            if (required > 0) transforms.reserve(required);

            mesh.mat.enable(VP);

            for (short modelid : mesh.models) 
                transforms.push(models[modelid].get_scaled());

            glAttribute(mesh.VAO)
                .bind<GL_ARRAY_BUFFER>(mesh.IBO)
                .data<GL_DYNAMIC_DRAW>(transforms)
                .draw<GL_UNSIGNED_SHORT>(mesh.indices.size(), transforms.size())
            ;
        }
        double render_time = drawtimer.stop() * 1000;
        std::cout << "drew " << kmesh << " mesh(es), " << kmodel << " model(s) in " << render_time << "ms\n";
    }
}