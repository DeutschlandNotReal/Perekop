#define PK_ENGINE_SRC 
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #define PK_DEBUG "render.cpp"

#include <PKLib/Math.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <type_traits>
#include <stb_image.h>

#include <Internal.hpp>
#include <PKLib/Time.hpp>
#include <PKLib/File.hpp>

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
        template <int L> glAttribute& gbuffer(GLuint* b) { glGenBuffers(L, b); return *this;}
        
        template <typename T> glAttribute& data(GLuint buffer, GLenum type, GLenum usage, arrayview<T> data) {
            glBindBuffer(type, buffer);
            glBufferData(type, data.size()*sizeof(T), data.begin(), usage);
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
            glDrawArraysInstanced(GL_TRIANGLES, 0, n_vert, n_users);
            return *this;
        }
};

GLuint load_shader(std::initializer_list<const char*> src, stringview title, GLenum T) {
    GLuint shader = glCreateShader(T);
    glShaderSource(shader, src.size(), src.begin(), 0);
    glCompileShader(shader);
    int ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[4096];
        glGetShaderInfoLog(shader, 4096, nullptr, log);
        printf("\033[31mShader '%s' (%s) failed to compile:\n%s\n\033[0m", title.begin(), (T == GL_VERTEX_SHADER ? "vertex" : "fragment"), log);
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
    uint8_t* data = stbi_load(path, &width, &height, &channels, 0);

    if (!data) {
        printf("\033[31mTexture at path '%s' not found.\n\033[0m", path);
        return;
    }

    int format = (channels==1)?GL_RED:(channels==3)?GL_RGB:GL_RGBA;
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

Texture::Texture(stringview path) {
    load_texture(&id, path);
};

Shader::Shader(stringview title, stringview vpath, stringview fpath) {
    string vsrc = File::read(vpath), fsrc = File::read(fpath);
    program = load_program({
        load_shader({Perekop::preamble_v, vsrc}, title, GL_VERTEX_SHADER), 
        load_shader({Perekop::preamble_f, fsrc}, title, GL_FRAGMENT_SHADER)
    });

    layoutP = glGetUniformLocation(program, "proj");
    layoutV = glGetUniformLocation(program, "view");
    layoutT = glGetUniformLocation(program, "f_image");
}

void Shader::uniform(Uniform type, stringview title, const void* data) {
    uniforms.push({
        glGetUniformLocation(program, title),
        type,
        data
    });
};

void Texture::use(uint layoutT) const {
    if (!id) return;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
    glUniform1i(layoutT, 0);
}

void Shader::use(const mat4& V, const mat4& P) const {
    if (!program) return;
    glUseProgram(program);
    glUniformMatrix4fv(layoutV, 1,GL_FALSE, (float*)&V);
    glUniformMatrix4fv(layoutP, 1, GL_FALSE, (float*)&P);

    for (const LUniform& u : uniforms) {
        using enum Uniform;
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
    if (VBO) return; // already loaded
    glAttribute()
        .gbuffer<3>(&VBO)
        .data<short>(EBO, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices)
        .data<Mesh::Vertex>(VBO, GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices);

    lbound = hbound = vertices[0].p;
    inertia = {0};
    for (const Mesh::Vertex& v : vertices) {
        lbound = min(lbound, v.p);
        hbound = max(hbound, v.p);
        inertia += Perekop::to_inertia(v.p);
    };
}

void Mesh::unload() {
    if (!VBO) return;
    glDeleteBuffers(3, &VBO);
    VBO = EBO = IBO = 0;
}

void Perekop::render(bool recollect) {
    using namespace World;
    vec2 wsize = Window::get_size();
    if (wsize.y == 0.f) return; // aspect ratio of inf (bad)

    glClearColor(bgcol.r, bgcol.g, bgcol.b, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 view = camera.view(), proj = camera.proj(wsize.x / wsize.y);
    glBindVertexArray(Perekop::mesh_VAO);

    // transform prealloc
    for (const Mesh& mesh : World::meshes) {
        if (mesh.id <= 0) continue; // invalid
        while (cache::T.size() < mesh.id) cache::T.emplace();

        if (recollect) cache::T[mesh.id-1].clear();
    }

    // transform collection
    if (recollect) for (const Model& model : World::models) {
        // model.mesh value of 0 means no mesh assigned
        if (!model.mesh || model.mesh > cache::T.size()) continue;
        // if mesh is part of body & isn't root part, mesh pose is treated as relative to root pose
        mat4 transform = model.pose.mat4(); 

        if (model.body) {
            const Body& mbody = World::bodies[model.body];
            if (model.id != mbody.rootid) transform *= mbody.pose.mat4();
        }

        cache::T[model.mesh-1].emplace(transform, model.metadata);
    }

    // instanced draw
    for (const Mesh& mesh : World::meshes) {
        if (mesh.id <= 0) continue;
        vector<ModelData>& modeldata = cache::T[mesh.id-1];
        if (modeldata.empty() || !mesh.shader) continue;

        mesh.shader->use(view, proj);
        mesh.texture.use(mesh.shader->layoutT);        

        glAttribute()
            .bind_elements(mesh.EBO)
            .data<ModelData>(mesh.IBO, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, modeldata)
            .vbuffer<Mesh::Vertex>(0, mesh.VBO)
            .vbuffer<ModelData>(1, mesh.IBO)
            .idraw(mesh.indices.size(), modeldata.size());
    }

    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(gui_PROG);

    // gui instanced draw
    if (!Gui::items.empty()) {
        cache::gui.reserve(Gui::items.size());
        cache::gui.clear();
        float minz{0}, maxz{1};

        for (const GUIObject& gui : Gui::items) {
            maxz = max(maxz, gui.Z); 
            minz = min(minz, gui.Z);
        }

        float iZR = 1.f / (maxz - minz);
        for (const GUIObject& gui : Gui::items)
            cache::gui.u_push({
            (gui.Z - minz) * iZR, 
            gui.pos, 
            gui.size, 
            gui.col
        });
        
        glAttribute(gui_VAO)
            .data<GuiData>(gui_IBO, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, cache::gui)
            .vbuffer<vec2>(0, gui_VBO)
            .vbuffer<GuiData>(1, gui_IBO)
            .idraw_array(6, cache::gui.size());
    }

    glfwSwapBuffers(glfw_window);
}

void Perekop::init_render() {
    glDepthFunc(GL_GREATER);
    glClearDepth(0.0);
    preamble_v = File::read("engine/assets/shaders/pre_vsrc.glsl");
    preamble_f = File::read("engine/assets/shaders/pre_fsrc.glsl");
    glAttribute(&mesh_VAO)
        // vertex (pos, norm, uv)
        .item<vec3, vec3, vec2>()
        // model (t, meta)
        .item_instanced<vec4, vec4, vec4, vec4, vec4>();
    
    vec2 gui_V[6] = {{0,0}, {0,1}, {1,0}, {1,0}, {0,1}, {1,1}};
    glAttribute(&gui_VAO)
        .gbuffer<1>(&gui_VBO).gbuffer<1>(&gui_IBO)
        .item<vec2>()
        .item_instanced<float, vec2, vec2, vec4>()
        .data<vec2>(gui_VBO, GL_ARRAY_BUFFER, GL_STATIC_DRAW, gui_V);
    
    string vsrc = File::read("engine/assets/shaders/gui_vert.glsl");
    gui_PROG = load_program({
        load_shader({vsrc}, "gui", GL_VERTEX_SHADER),
        load_shader({"#version 430\n in vec4 col2; out vec4 fragColor; void main() { fragColor = col2; }"}, "gui", GL_FRAGMENT_SHADER)
    });
    
}