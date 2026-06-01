#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <type_traits>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

void load_texture(GLuint* texture, const char* path) {
    int width, height, channels;
    unsigned char *data = stbi_load(path, &width, &height, &channels, 0);
    if (!data) {
        printf("\033[31mTexture at path '%s' not found.\n\033[0m", path); return;
    }
    int format = (channels==1)?GL_RED:(channels==3)?GL_RGB:GL_RGBA;
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

Mesh::Appearance::Appearance(const char* vpath, const char* fpath, const char* tpath) {
    const char *vsrc = File::read(vpath), *fsrc = File::read(fpath);
    program = load_program({
        load_shader({Perekop::preamble_v, vsrc}, "vertex", GL_VERTEX_SHADER), 
        load_shader({Perekop::preamble_f, fsrc}, "fragment", GL_FRAGMENT_SHADER)
    });
    delete[] vsrc; delete[] fsrc;  
    layoutP = glGetUniformLocation(program, "P");
    layoutV = glGetUniformLocation(program, "V");
    if (!tpath) return;
    layoutT = glGetUniformLocation(program, "_texture");

    load_texture(&texture, tpath);
}

void Mesh::Appearance::uniform(Mesh::UniType T, const char* title, const void* data) {
    uniforms.push({
        glGetUniformLocation(program, title),
        T,
        data
    });
};

void Mesh::Appearance::use(const mat4& V, const mat4& P) const {
    glUseProgram(program);
    glUniformMatrix4fv(layoutV, 1,GL_FALSE, (float*)&V);
    glUniformMatrix4fv(layoutP, 1, GL_FALSE, (float*)&P);

    if (texture && layoutT) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture); 
        glUniform1i(layoutT, 0);
    }
    for (const Uniform& u : uniforms) {
        switch (u.type) {
            case u_mat3:
                glUniformMatrix3fv(u.layout, 1, false, (float*)u.data); break;
            case u_mat4:
                glUniformMatrix4fv(u.layout, 1, false, (float*)u.data); break;
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
    if (VBO) return;
    glAttribute().make<3>(&VBO);
    reload();
}

void Mesh::reload() {
    if (!VBO) load();
    glAttribute()
        .data<GL_ARRAY_BUFFER, GL_STATIC_DRAW>(VBO, vertex.begin(), vertex.size())
        .data<GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW>(EBO, indices.begin(), indices.size());
}

void Mesh::unload() {
    if (!VBO) return;
    glDeleteBuffers(3, &VBO);
    VBO = EBO = IBO = 0;
}

void Perekop::render(bool recollect) {
    using namespace World;
    if (Window::size.y == 0) return; // aspect ratio of inf (bad)

    glClearColor(bgcol.r, bgcol.g, bgcol.b, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 view = camera.view(), proj = camera.proj(Window::size.x / Window::size.y);

    glBindVertexArray(Perekop::mesh_VAO);

    // transform prealloc
    for (const Mesh& mesh : World::meshes) {
        if (mesh.id <= 0) continue; // invalid
        while (transforms.size() < mesh.id)
            transforms.emplace(8);

        transforms[mesh.id-1].clear();
    }

    // transform collection
    if (recollect) for (const Model& model : World::models) {
        // model.mesh value of 0 means no mesh assigned
        if (!model.mesh || model.mesh-1 >= transforms.size()) continue;
        mat4 mdata = model.body ? model.pose * World::bodies[model.body].pose : model.pose;

        mdata[3] = model.metadata;
        transforms[model.mesh-1].push(mdata);
    }

    // instanced draw
    for (const Mesh& mesh : World::meshes) {
        Array<mat4>& T = transforms[mesh.id-1];
        if (T.empty()) continue;
        mesh.appearance->use(view, proj);
        glAttribute()
            .bind_elements(mesh.EBO)
            .data<GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW>(mesh.IBO, T.begin(), T.size())
            .vbuffer<Mesh::Vertex>(0, mesh.VBO)
            .vbuffer<mat4>(1, mesh.IBO)
            .idraw(mesh.indices.size(), T.size());
    }

    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(gui_PROG);

    // gui instanced draw
    if (!GUI::gui.empty()) {
        guidata.reserve(GUI::gui.size());
        guidata.clear();
        float minz{0}, maxz{1};

        for (const GUIObject& gui : GUI::gui) {
            maxz = max(maxz, gui.Z); 
            minz = min(minz, gui.Z);
        }

        float iZR = 1.f / (maxz - minz);
        for (const GUIObject& gui : GUI::gui)
            guidata.rawpush({
            (gui.Z - minz) * iZR, 
            gui.pos, 
            gui.size, 
            gui.col
        });
        
        glAttribute(gui_VAO)
            .data<GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW>(gui_IBO, guidata.begin(), guidata.size())
            .vbuffer<vec2>(0, gui_VBO)
            .vbuffer<rawgui>(1, gui_IBO)
            .idraw_array(6, guidata.size());
    }

    glfwSwapBuffers(glfw_window);
}

void Perekop::init_render() {
    preamble_v = File::read("pkengine/assets/shaders/pre_vsrc.glsl");
    preamble_f = File::read("pkengine/assets/shaders/pre_fsrc.glsl");
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
        load_shader({"#version 430\n in vec4 col2; out vec4 fragColor; void main() { fragColor = col2; }"}, "gui_fshader", GL_FRAGMENT_SHADER)
    });
    delete[] vsrc;
}