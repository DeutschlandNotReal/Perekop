#include "Perekop.hpp"
#include <cstdlib>
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <type_traits>

#include <Internal.hpp>
#include <pkutil/Time.hpp>
#include <pkutil/File.hpp>
using namespace glm;
using namespace pk;
 
class glAttribute {
    int index{0};
    template <typename T> void member(int d, int s, long long& o) {
        if constexpr(std::is_same_v<T, float>) {
            glVertexAttribPointer(index, 1, GL_FLOAT, 0, s, (void*)o);
        } else if constexpr(std::is_same_v<T, vec4> || std::is_same_v<T, vec3> || std::is_same_v<T, vec2>) 
            glVertexAttribPointer(index, T::length(), GL_FLOAT, 0, s, (void*)o);
        else if constexpr(std::is_same_v<T, int>)
            glVertexAttribIPointer(index, 1, GL_INT, s, (void*)o);
        else if constexpr(std::is_same_v<T, uint>)
            glVertexAttribIPointer(index, 1, GL_UNSIGNED_INT, s, (void*)o);
        o += sizeof(T);
        glEnableVertexAttribArray(index);
        glVertexAttribDivisor(index++, d);
    }

    public:
        glAttribute() = default;
        glAttribute(GLuint array) { glBindVertexArray(array); }
        glAttribute(GLuint* array) { glGenVertexArrays(1, array); glBindVertexArray(*array); }
        template <int d, typename... T> glAttribute& item() {
            int stride = (sizeof(T) + ...); long long offset = 0;
            (member<T>(d, stride, offset), ...);
            return *this;
        }
        template <GLenum type> glAttribute& bind(GLuint b) { glBindBuffer(type, b); return *this; }
        template <int L> glAttribute& make(GLuint* b) { glGenBuffers(L, b); return *this;}
        
        template <GLenum type, GLenum mode, typename T> glAttribute& data(GLuint buffer, const T* data, int n) {
            glBindBuffer(type, buffer);
            glBufferData(type, n*sizeof(T), data, mode);
            return *this;
        }

        template <GLenum type> glAttribute& idraw(int n_ind, int n_users) {
            glDrawElementsInstanced(GL_TRIANGLES, n_ind, type, 0, n_users);
            return *this;
        }

        glAttribute& idraw_array(int n_vert, int n_users) {
            glad_glDrawArraysInstanced(GL_TRIANGLES, 0, n_vert, n_users);
            return *this;
        }
};

GLuint load_shader(std::initializer_list<const char*> src, const char* title, GLenum T) {
    GLuint shader = glCreateShader(T);
    glShaderSource(shader, src.size(), src.begin(), NULL);
    glCompileShader(shader);
    int ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[4096];
        glGetShaderInfoLog(shader, 4096, nullptr, log);
        printf("\033[31mShader '%s' failed to compile:\n%s\n\033[0m", title, log);
    }
    return shader;
}

GLuint load_program(std::initializer_list<GLuint> shaders) {
    GLuint program(glCreateProgram());
    for (GLuint s : shaders) glAttachShader(program, s);
    glLinkProgram(program);
    for (GLuint s : shaders) glDeleteShader(s);
    return program;
}

Mesh::Material::Material(const char* vsrc, const char* fsrc) {
    program = load_program({
        load_shader({Perekop::preamble_v, vsrc}, "vertex", GL_VERTEX_SHADER), 
        load_shader({Perekop::preamble_f, fsrc}, "fragment", GL_FRAGMENT_SHADER)
    });
    layout_V = glGetUniformLocation(program, "V");
    layout_P = glGetUniformLocation(program, "P");
}

void Mesh::Material::uniform(Mesh::UniType T, const char* title, const void* data) {
    uniforms.push({
        glGetUniformLocation(program, title),
        T,
        data
    });
};

void Mesh::Material::use(const mat4& V, const mat4& P) const {
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

void Mesh::load() {
    if (VAO) return;
    glAttribute(&VAO)
        .make<3>(&VBO)
        .bind<GL_ELEMENT_ARRAY_BUFFER>(EBO)
        .bind<GL_ARRAY_BUFFER>(VBO).item<0, vec3, vec3, vec2>() // VERTEX
        .bind<GL_ARRAY_BUFFER>(IBO).item<1, vec4, vec4, vec4>(); // MODEL
    reload();
}

void Mesh::reload() {
if (!VAO) load();
    glAttribute(VAO)
        .data<GL_ARRAY_BUFFER, GL_STATIC_DRAW>(VBO, vertex.begin(), vertex.size())
        .data<GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW>(EBO, indices.begin(), indices.size());
}

void Mesh::unload() {
    if (!VAO) return;
    glDeleteBuffers(3, &VBO);
    glDeleteVertexArrays(1, &VAO);
    VAO = VBO = EBO = IBO = 0;
}

void Perekop::draw() {
    vec2 screendim = Window::size();
    if (screendim.y == 0) return;

    glClearColor(.1, .1, .1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const mat4 
        V = inverse((glm::mat4)World::camera.pose),
        P = perspective(
            radians(World::camera.fov), 
            screendim.x / screendim.y,
            World::camera.min, World::camera.max
        );

    for (const Mesh& mesh : World::meshes) {
        if (mesh.models.size() == 0 || !mesh.mat || !mesh.mat->program) continue;
        transforms.clear();
        transforms.reserve(mesh.models.size());
        mesh.mat->use(V, P);

        for (short modelid : mesh.models) 
            transforms.rawpush((mat3x4)World::models[modelid].pose);

        glAttribute(mesh.VAO)
            .data<GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW>(mesh.IBO, transforms.begin(), transforms.size())
            .idraw<GL_UNSIGNED_SHORT>(mesh.indices.size(), transforms.size());
    }

    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(gui_PROG);
    glAttribute(gui_VAO)
        .data<GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW>(gui_IBO, Window::gui.begin(), Window::gui.size())
        .idraw_array(6, Window::gui.size());

    glfwSwapBuffers(glfw_window);
}

void Perekop::init::render() {
    preamble_v = File::read("pkengine/assets/shaders/pre_vsrc.glsl");
    preamble_f = File::read("pkengine/assets/shaders/pre_fsrc.glsl");
    vec2 gui_V[6] = {{0,0}, {0,1}, {1,0}, {1,0}, {0,1}, {1,1}};
    glAttribute(&gui_VAO)
        .make<1>(&gui_VBO)
        .make<1>(&gui_IBO)
        .bind<GL_ARRAY_BUFFER>(gui_VBO).item<0, vec2>().data<GL_ARRAY_BUFFER, GL_STATIC_DRAW>(gui_VBO, gui_V, 6)
        .bind<GL_ARRAY_BUFFER>(gui_IBO).item<1, float, vec2, vec2, vec3>();
    const char* vsrc = File::read("pkengine/assets/shaders/gui_vert.glsl");
    gui_PROG = load_program({
        load_shader({vsrc}, "gui_vshader", GL_VERTEX_SHADER),
        load_shader({"#version 330\n in vec3 col2; out vec4 fragColor; void main() { fragColor = vec4(col2, 1.0); }"}, "gui_fshader", GL_FRAGMENT_SHADER)
    });
    delete[] vsrc;
}