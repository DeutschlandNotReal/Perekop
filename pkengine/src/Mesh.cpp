#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <Perekop/Engine.hpp>
#include <Perekop/Mesh.hpp>
#include <Perekop/Model.hpp>

#include <PKInternal/workers.hpp>

using namespace glm;
using namespace Perekop;
using namespace PKInternal;

GLuint load_shader(const char** src, GLenum type) {
    // should probably make it detect sloppy shader code
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 2, src, NULL);
    glCompileShader(shader);
    return shader;
}

GLuint load_program(const char* pre_vsrc, const char* pre_fsrc, const char* vsrc, const char* fsrc) {
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
    long long stride = 0;
    int index = 0, offset = 0;
    template <typename T> inline attrib_builder& object() {
        offset = sizeof(T);
        stride = 0;
        return *this;
    }
    template <typename T> inline attrib_builder& member(int divisor = 0) {
        glVertexAttribPointer(index, sizeof(T)/4, GL_FLOAT, GL_FALSE, offset, (void*)stride);
        glEnableVertexAttribArray(index);
        stride += sizeof(T);
        glVertexAttribDivisor(index++, divisor);
        return *this;
    }
    inline attrib_builder& bind_array(GLuint buffer) {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        return *this;
    }
    inline attrib_builder& bind_element_array(GLuint buffer) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        return *this;
    }
};

namespace pk {
    MeshMaterial::MeshMaterial(const char* vsrc, const char* fsrc) {
        Workers::render.add_work([=](){
            const char* pre_vshader = "#version 330"
                "\n layout(location = 0) in vec3 _pos;"
                "\n layout(location = 1) in vec2 _uv;"
                "\n layout(location = 2) in vec4 M0;"
                "\n layout(location = 3) in vec4 M1;"
                "\n layout(location = 4) in vec4 M2;"
                "\n uniform mat4 VP;"
                "\n mat4 model() {"
                "\n     return mat4(vec4(M0.xyz, 0.0), vec4(M1.xyz, 0.0), vec4(M2.xyz, 0.0), vec4(M0.w, M1.w, M2.w, 1.0)); "
                "\n }";
            
            const char* pre_fshader = "#version 330 \n out vec4 fragColor;";
            program = load_program(pre_vshader, pre_fshader, vsrc, fsrc);
        });
    }

    void MeshMaterial::use(const glm::mat4& VP) {
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
        Workers::render.add_work([this](){
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(MeshVertex), vertex, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangle_count * sizeof(MeshTriangle), triangle, GL_STATIC_DRAW);
        });
    }

    void Mesh::load() {
        if (VAO) return;
        Workers::render.add_work([this](){
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            glGenBuffers(1, &IBO);
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            attrib_builder()
                .bind_array(VBO)
                .object<MeshVertex>()
                .member<glm::vec3>()
                .member<glm::vec2>()
                .bind_array(IBO)
                .bind_element_array(EBO)
                .object<glm::mat3x4>()
                .member<glm::vec4>(1)
                .member<glm::vec4>(1)
                .member<glm::vec4>(1)
            ;

            glBindVertexArray(0);
        });
    }

    void Mesh::unload() {
        if (!VAO) return;
        Workers::render.add_work([this](){
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            glDeleteBuffers(1, &IBO);
            glDeleteVertexArrays(1, &VAO);
            VAO = 0, VBO = 0, EBO = 0, IBO = 0;
        });
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
        delete[] users; 
        delete[] vertex; 
        delete[] triangle;
        if (!renderer) return;
        Mesh* back = renderer->meshes[--renderer->meshes_count];
        renderer->meshes[meshid] = back;
        back->meshid = meshid;
    }

    void MeshRenderer::draw() {
        // user probably maybe wont call this (no need to check render owrker)
        int screenx, screeny;
        Perekop::Window::get_size(screenx, screeny);
        const glm::mat4 VP = Perekop::Scene::camera.get_viewproj(screenx, screeny);
        ID_T& tcur = transforms_count, tsize = transforms_capacity;
        for (ID_T i = 0; i < meshes_count; i++) {
            Mesh& mesh = *(meshes[i]);
            if (mesh.material.program == 0) continue;;
            mesh.material.use(VP);
            glBindVertexArray(mesh.VAO);
            if (mesh.users_count > tsize) {
                delete[] transforms;
                tsize = mesh.users_count;
                transforms = new mat3x4[tsize];
            }
            for (tcur = 0; tcur < mesh.users_count; tcur++) {
                Model* model = mesh.users[tcur];
                transforms[tcur] = model->transform.scale(model->scl);
            }
            glBindBuffer(GL_ARRAY_BUFFER, mesh.IBO);
            glBufferData(GL_ARRAY_BUFFER, tcur * sizeof(glm::mat3x4), transforms, GL_DYNAMIC_DRAW);
            glDrawElementsInstanced(GL_TRIANGLES, mesh.triangle_count * 3, GL_UNSIGNED_SHORT, 0, mesh.users_count);
        }
        glBindVertexArray(0);
    }

    Mesh* MeshRenderer::create_mesh(MeshMaterial material) {
        Mesh* mesh = new Mesh(material);
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
}