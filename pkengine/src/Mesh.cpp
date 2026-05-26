#include "glm/matrix.hpp"
#include <pk/glAttribute.hpp>
#include <glm/glm.hpp>
#include <glm/glm.hpp>

#include <iostream>
#include <pk/Scene.hpp>
#include <pk/Engine.hpp>
#include <pkutil/Time.hpp>
#include <pkutil/File.hpp>
using namespace glm;

GLuint load_shader(std::initializer_list<const char*> src, const char* title, GLenum T) {
    GLuint shader = glCreateShader(T);
    glShaderSource(shader, src.size(), src.begin(), NULL);
    glCompileShader(shader);
    int ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[4096];
        glGetShaderInfoLog(shader, 4096, nullptr, log);
        std::cout << "\033[31mShader '" << title << "' failed to compile:\n" << log << "\n\033[0m";
    }
    return shader;
}

namespace pk {
    glm::mat4 Camera::projection(glm::vec2 screen_size) const {
        return glm::perspective(
            glm::radians(fov), 
            screen_size.x / screen_size.y,
            n, f
        );
    }

    glm::mat4 Camera::view() const {
        return glm::inverse((glm::mat4)transform);
    }

    Mesh::Material::Material(const char* vsrc, const char* fsrc) {
        GLuint v = load_shader({v_preamble, vsrc}, "vshader", GL_VERTEX_SHADER),
               f = load_shader({f_preamble, fsrc}, "fshader", GL_FRAGMENT_SHADER);
        program = glCreateProgram();
        glAttachShader(program, v);
        glAttachShader(program, f);
        glLinkProgram(program);
        glDeleteShader(v);
        glDeleteShader(f);
        layout_V = glGetUniformLocation(program, "V");
        layout_P = glGetUniformLocation(program, "P");
    }

    void Mesh::Material::add_uniform(Mesh::UniType T, const char* title, const void* data) {
        uniforms.push({
            glGetUniformLocation(program, title),
            T,
            data
        });
    };

    void Mesh::Material::use(const glm::mat4& V, const glm::mat4& P) const {
        glUseProgram(program);
        glUniformMatrix4fv(layout_V, 1,GL_FALSE, (float*)&V);
        glUniformMatrix4fv(layout_P, 1, GL_FALSE, (float*)&P);

        for (const auto& u : uniforms) {
            switch (u.type) {
                case u_mat3:
                    glUniformMatrix3fv(u.layout, 1, GL_FALSE, (float*)u.data); break;
                case u_mat4:
                    glUniformMatrix4fv(u.layout, 1, GL_FALSE, (float*)u.data); break;
                case u_float:
                    glUniform1f(u.layout, *(float*)u.data); break;
                case u_int:
                    glUniform1i(u.layout, *(int*)u.data); break;
                case u_vec2:
                    glUniform2fv(u.layout, 1, (float*)u.data); break;
                case u_vec3:
                    glUniform3fv(u.layout, 1, (float*)u.data); break;
                case u_vec4:
                    glUniform4fv(u.layout, 1, (float*)u.data); break;
            }
        }
    }

    void Mesh::refresh() {
        if (!VAO) load();
        glAttribute(VAO)
            .data<GL_ARRAY_BUFFER, GL_STATIC_DRAW>(VBO, vertex)
            .data<GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW>(EBO, indices);
    }

    void Mesh::load() {
        if (VAO) return;
        glAttribute(&VAO)
            .make<3>(&VBO)
            .bind<GL_ELEMENT_ARRAY_BUFFER>(EBO)
            .bind<GL_ARRAY_BUFFER>(VBO)
            .object<0>(3, 3, 2) // vertex (pos, normal, uv)
            .bind<GL_ARRAY_BUFFER>(IBO)
            .object<1>(4, 4, 4); // model matrix
    }

    void Mesh::unload() {
        if (!VAO) return;
        glDeleteBuffers(3, &VBO);
        glDeleteVertexArrays(1, &VAO);
        VAO = VBO = EBO = IBO = 0;
    }

    void Scene::draw(const Window& window) {
        window.set_context();
        glClearColor(.1, .1, .1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        const mat4 V = Perekop::camera.view();
        const mat4 P = Perekop::camera.projection(window.size());

        for (const Mesh& mesh : meshes) {
            if (mesh.models.size() == 0 || !mesh.mat || !mesh.mat->program) continue;
            transforms.clear();
            transforms.reserve(mesh.models.size());
            mesh.mat->use(V, P);

            for (short modelid : mesh.models) 
                transforms.rawpush(models[modelid].get_scaled());

            glAttribute(mesh.VAO)
                .data<GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW>(mesh.IBO, transforms)
                .draw<GL_UNSIGNED_SHORT>(
                    mesh.indices.size(), 
                    transforms.size()
                );
        }
    }
}