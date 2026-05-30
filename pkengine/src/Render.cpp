#include "Perekop.hpp"
#include "pk/GUI.hpp"
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
    template <typename T> void member(int d, int& o) {
        if constexpr(std::is_same_v<T, float>) {
            glVertexAttribFormat(index, 1, GL_FLOAT, 0, o);
        } else if constexpr(std::is_same_v<T, vec4> || std::is_same_v<T, vec3> || std::is_same_v<T, vec2>) 
            glVertexAttribFormat(index, T::length(), GL_FLOAT, 0, o);
        else if constexpr(std::is_same_v<T, int>)
           glVertexAttribIFormat(index, 1, GL_INT, o);
        else if constexpr(std::is_same_v<T, uint>)
           glVertexAttribIFormat(index, 1, GL_UNSIGNED_INT, o);
        glVertexAttribBinding(index, d);
        glEnableVertexAttribArray(index++);
        o += sizeof(T);
    }

    public:
        glAttribute() = default;
        glAttribute(GLuint array) { glBindVertexArray(array); }
        glAttribute(GLuint* array) { glGenVertexArrays(1, array); glBindVertexArray(*array); }
        template <typename... T> glAttribute& item() {
            glVertexBindingDivisor(0, 0);
            int offset{0};
            (member<T>(0, offset), ...);
            return *this;
        }
        template <typename... T> glAttribute& item_instanced() {
            glVertexBindingDivisor(1, 1);
            int offset{0};
            (member<T>(1, offset), ...);
            return *this;
        }

        glAttribute& bind_elements(GLuint b) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b); return *this;}
        template <int L> glAttribute& make(GLuint* b) { glGenBuffers(L, b); return *this;}
        
        template <GLenum type, GLenum mode, typename T> glAttribute& data(GLuint buffer, const T* data, int n) {
            glBindBuffer(type, buffer);
            glBufferData(type, n*sizeof(T), data, mode);
            return *this;
        }

        template <typename T> glAttribute& vbuffer(int divisor, GLuint buffer) {
            glBindVertexBuffer(divisor, buffer, 0, sizeof(T));
            return *this;
        }

        glAttribute& idraw(int n_ind, int n_users) {
            glDrawElementsInstanced(GL_TRIANGLES, n_ind, GL_UNSIGNED_SHORT, 0, n_users);
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
        load_shader({fsrc}, "fragment", GL_FRAGMENT_SHADER)
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

void Mesh::r_load() {
    if (VBO) return;
    glAttribute().make<3>(&VBO);
    r_reload();
}

void Mesh::r_reload() {
    if (!VBO) r_load();
    glAttribute()
        .data<GL_ARRAY_BUFFER, GL_STATIC_DRAW>(VBO, vertex.begin(), vertex.size())
        .data<GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW>(EBO, indices.begin(), indices.size());
}

void Mesh::r_unload() {
    if (!VBO) return;
    glDeleteBuffers(3, &VBO);
    VBO = EBO = IBO = 0;
}

void Mesh::load() { flags = 1; }
void Mesh::reload() { flags = 2; }
void Mesh::unload() { flags = 3; }

void Perekop::draw() {
    vec2 screendim = Window::size();
    if (screendim.y == 0) return;

    const vec3 bg = World::background_colour;
    glClearColor(bg.x, bg.y, bg.z, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const mat4 
        V = inverse((glm::mat4)World::camera.pose),
        P = perspective(
            radians(World::camera.fov), 
            screendim.x / screendim.y,
            World::camera.min, World::camera.max
        );

    glBindVertexArray(Perekop::mesh_VAO);
    for (Mesh& mesh : World::meshes) {
        switch (mesh.flags) {
            case 1: mesh.r_load(); break;
            case 2: mesh.r_reload(); break;
            case 3: mesh.r_unload(); break;
        }
        mesh.flags = 0;
        if (mesh.models.size() == 0 || !mesh.mat || !mesh.mat->program) continue;
        transforms.clear();
        transforms.reserve(mesh.models.size());
        mesh.mat->use(V, P);

        for (short modelid : mesh.models) {
            Model& m = World::models[modelid];
            mat3x4 mat = m.pose;
            transforms.push({mat[0], mat[1], mat[2], m.metadata});
        }

        glAttribute()
            .bind_elements(mesh.EBO)
            .data<GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW>(mesh.IBO, transforms.begin(), transforms.size())
            .vbuffer<Mesh::Vertex>(0, mesh.VBO)
            .vbuffer<mat4>(1, mesh.IBO)
            .idraw(mesh.indices.size(), transforms.size());
    }

    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(gui_PROG);

    if (GUI::gui.size() > 0) {
        if (GUI::gui.size() > guidata.capacity()) guidata.reserve(GUI::gui.size());
        guidata.clear();
        float minz{GUI::gui[0].Z}, maxz{minz};
        for (int i = 1; i < guidata.size(); i++) {
            float Z = GUI::gui[i].Z;
            maxz = max(maxz, Z); minz = min(minz, Z);
        }
        if (GUI::gui.size() == 1) minz = 0;
        for (const GUIObject& gui : GUI::gui) {
            float Z = (gui.Z - minz) / (maxz - minz) - .5f;
            guidata.rawpush({Z, gui.position, gui.size, gui.colour});
        }

        glAttribute(gui_VAO)
            .data<GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW>(gui_IBO, guidata.begin(), guidata.size())
            .vbuffer<vec2>(0, gui_VBO)
            .vbuffer<rawgui>(1, gui_IBO)

            .idraw_array(6, guidata.size()); 
        }
        if (GUI::gui.size() > guidata.capacity()) guidata.reserve(GUI::gui.size());
        guidata.clear();

    glAttribute(gui_VAO)
        .data<GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW>(gui_IBO, guidata.begin(), guidata.size())
        .vbuffer<vec2>(0, gui_VBO)
        .vbuffer<rawgui>(1, gui_IBO)

        .idraw_array(6, guidata.size());

    glfwSwapBuffers(glfw_window);
}

void Perekop::init::render() {
    preamble_v = File::read("pkengine/assets/shaders/pre_vsrc.glsl");
    glAttribute(&mesh_VAO)
        .item<vec3, vec3, vec2>() // VERTEX
        .item_instanced<vec4, vec4, vec4, vec4>(); // MODEL
    
    vec2 gui_V[6] = {{0,0}, {0,1}, {1,0}, {1,0}, {0,1}, {1,1}};
    glAttribute(&gui_VAO)
        .make<1>(&gui_VBO).make<1>(&gui_IBO)
        .item<vec2>()
        .item_instanced<float, vec2, vec2, vec4>()
        .data<GL_ARRAY_BUFFER, GL_STATIC_DRAW>(gui_VBO, gui_V, 6);
    
    const char* vsrc = File::read("pkengine/assets/shaders/gui_vert.glsl");
    gui_PROG = load_program({
        load_shader({vsrc}, "gui_vshader", GL_VERTEX_SHADER),
        load_shader({"#version 330\n in vec4 col2; out vec4 fragColor; void main() { fragColor = col2; }"}, "gui_fshader", GL_FRAGMENT_SHADER)
    });
    delete[] vsrc;
}