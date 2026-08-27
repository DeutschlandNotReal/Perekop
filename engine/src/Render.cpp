#define PK_INTERNAL
#define CGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include <PKINT/internal.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <cgltf.h>
#include <iostream>

#include <PK/file.hpp>

using namespace pk;
using std::string_view;
using std::string;

class glAttribute {
    int index{0};
    template <typename T> void member(int d, int& o) {
        #define is_type(t) std::is_same_v<T, t>

        if constexpr (is_type(float) || is_type(quat) || is_type(vec4) || is_type(vec3) || is_type(vec2))
            glVertexAttribFormat(index, sizeof(T) >> 2, GL_FLOAT, 0, o);
        else if constexpr(is_type(int) || is_type(unsigned))
            glVertexAttribFormat(index, 1, is_type(int) ? GL_INT : GL_UNSIGNED_INT, o);

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
        
        template <typename T> glAttribute& data(GLuint buffer, GLenum type, GLenum usage, span<T> data) {
            glBindBuffer(type, buffer);
            glBufferData(type, data.size() * sizeof(T), data.begin(), usage);

            return *this;
        }

        template <typename T> glAttribute& vbuffer(int divisor, GLuint buffer) {
            glBindVertexBuffer(divisor, buffer, 0, sizeof(T));
            return *this;
        }

        glAttribute& idraw(int indcount, int usecount) {
            glDrawElementsInstanced(GL_TRIANGLES, indcount, GL_UNSIGNED_SHORT, 0, usecount);
            return *this;
        }

        glAttribute& idraw_array(int n_vert, int n_users) {
            glDrawArraysInstanced(GL_TRIANGLES, 0, n_vert, n_users);
            return *this;
        }
};


GLuint load_shader(const string& src, GLenum T) {
    GLuint shader = glCreateShader(T);
    const char* data = src.data();
    int size = src.size();

    glShaderSource(shader, 1, &data, &size);
    glCompileShader(shader);

    int ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[4096];
        glGetShaderInfoLog(shader, 4096, nullptr, log);
        printf("\033[31mShader failed to compile:\n%s\n\033[0m", log);
    }
    return shader;
}

unsigned int
    meshVAO,
    meshVBO,
    guiVAO,
    guiVBO,
    guiIBO,
    guiShader;

GLuint load_program(std::initializer_list<GLuint> shaders) {
    GLuint program(glCreateProgram());
    for (GLuint s : shaders) glAttachShader(program, s);
    glLinkProgram(program);
    for (GLuint s : shaders) glDeleteShader(s);
    return program;
}

void load_texture(unsigned* txtid, const path& path) {
    int width, height, channels;

    uint8_t* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

    if (!data) return;

    int format = (channels==1) ? GL_RED : (channels==3) ? GL_RGB : GL_RGBA;
    glGenTextures(1, txtid);
    glBindTexture(GL_TEXTURE_2D, *txtid);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

void Perekop::World::import_scene(
    const std::filesystem::path& path, vector<set_handle<Model>>* emited_models, vector<set_handle<Mesh>>* emited_meshes
) {
    cgltf_options options{};  
    cgltf_data* data{nullptr};
    auto spath = path.string();

    cgltf_result result = cgltf_parse_file(&options, spath.c_str(), &data);

    result = cgltf_load_buffers(&options, data, spath.c_str());

    for (unsigned id = 0; id < data->meshes_count; id++) {
        cgltf_mesh& gmesh = data->meshes[id];
        
        // each primitive is its own mesh
        for (unsigned pid = 0; pid < gmesh.primitives_count; pid++) {
            cgltf_accessor *pos{nullptr}, *nor{nullptr}, *uv{nullptr};
            const auto& primitive = gmesh.primitives[pid];

            for (unsigned attid = 0; attid < primitive.attributes_count; attid++) {
                const auto& attribute = primitive.attributes[attid];

                switch (attribute.type) {
                    case cgltf_attribute_type_position: pos = attribute.data; break;
                    case cgltf_attribute_type_normal: nor = attribute.data; break;
                    case cgltf_attribute_type_texcoord: uv = attribute.data; break;
                    default: 
                }
            }
            if (!pos || pos->count == 0) continue;
            
            decltype(Mesh::vertices) vertices; 
            decltype(Mesh::indices) indices;
            
            vertices.reserve(pos->count);
            for (unsigned vid = 0; vid < pos->count; vid++) {
                Mesh::Vertex &v = vertices.emplace();
                cgltf_accessor_read_float(pos, vid, &v.p.x, 3);

                if (nor) cgltf_accessor_read_float(nor, vid, &v.n.x, 3);
                if (uv) cgltf_accessor_read_float(uv, vid, &v.uv.x, 2);
            }

            if (primitive.indices) {
                indices.reserve(primitive.indices->count);
                for (unsigned iid = 0; iid < primitive.indices->count; iid++) {
                    unsigned int intermediate;
                    cgltf_accessor_read_uint(primitive.indices, iid, &intermediate, 1);
                    indices.push(intermediate);
                }
            } else {
                indices.reserve(vertices.size());
                for (unsigned vid = 0; vid < vertices.size(); vid++) indices.push(vid);
            }

            set_handle<Mesh> handle = World::meshes.insert(std::move(vertices), std::move(indices));
            if (emited_meshes) {
                emited_meshes->push(std::move(handle));
            }
        }
    }
}

Texture::Texture(const path& path) {
    load_texture(&txtid, path);
};

Shader::Shader(const path& vpath, const path& fpath) {
    string vsrc = file(vpath), fsrc = file(fpath);
    program = load_program({
        load_shader(vsrc, GL_VERTEX_SHADER), 
        load_shader(fsrc, GL_FRAGMENT_SHADER)
    });
  
    layoutP = glGetUniformLocation(program, "proj");
    layoutV = glGetUniformLocation(program, "view");
    layoutT = glGetUniformLocation(program, "f_image");
}

void Shader::uniform(UniformType type, string_view title, const void* data) {
    uniforms.emplace(
        (unsigned short) glGetUniformLocation(program, title.begin()),
        type,
        data
    );
};

void Texture::use(unsigned layoutT) const noexcept {
    if (txtid) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, txtid);
        glUniform1i(layoutT, 0);
    }
}

void Shader::use(const mat4 &V, const mat4 &P) const noexcept {
    if (!program) return;
    glUseProgram(program);
    glUniformMatrix4fv(layoutV, 1,GL_FALSE, (float*)&V);
    glUniformMatrix4fv(layoutP, 1, GL_FALSE, (float*)&P);

    for (const Uniform& u : uniforms) {
        using enum UniformType;
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
    glAttribute(meshVAO)
        .gbuffer<3>(&VBO)
        .data<uint16_t>(EBO, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices)
        .data<Mesh::Vertex>(VBO, GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices)
        .vbuffer<Mesh::Vertex>(0, VBO);
}

void Mesh::unload() {
    glDeleteBuffers(3, &VBO);
    VBO = EBO = IBO = 0;
}

// recollect: gather all model transforms, convert to mat4 for shader
void Perekop::render(bool recollect) {
    using namespace World;
    vec2 wsize = Window::get_size();

    if (wsize.y * wsize.x == 0.f) return;

    glClearColor(bgcol.x, bgcol.y, bgcol.z, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 view = camera.view(), proj = camera.proj(wsize.x, wsize.y);
    glBindVertexArray(meshVAO);

    // transform prealloc
    cache::T.reserve(World::meshes.size());

    while (cache::T.size() < World::meshes.size())
        cache::T.emplace();
 
    if (recollect) {
        // old transform dispose
        for (auto &modeldata : cache::T) modeldata.clear();

        // transform collection
        for (const Model &model : World::models) {
            cache::T[model.mesh_id - 1].emplace(model.pose, model.scale, model.metadata);
        }
    }

    // instanced draw
    for (Mesh& mesh : World::meshes) {
        vector<ModelData> &modeldata = cache::T[mesh.id - 1];

        if (modeldata.is_empty() && mesh.loaded()) {
            mesh.unload(); continue; // lazy unload
        } else if (mesh.shader) {
            if (!mesh.loaded()) mesh.load(); // lazy load
        }

        if (!mesh.shader) continue;

        mesh.shader->use(view, proj);

        if (mesh.texture) mesh.texture->use(mesh.shader->layoutT); 
 
        glAttribute(meshVAO)
            .bind_elements(mesh.EBO)
            .data<ModelData>(mesh.IBO, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, modeldata)
            .vbuffer<ModelData>(1, mesh.IBO)
            .idraw(mesh.indices.size(), modeldata.size());
    }

    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(guiShader);

    // gui instanced draw
    /*
    if (!Gui::items.is_empty()) {
        cache::gui.reserve(Gui::items.size());
        cache::gui.clear();
        f32 minz{0.f}, maxz{1.f};

        for (const gui_instance &gui : Gui::items) {
            maxz = max(maxz, gui.Z); 
            minz = min(minz, gui.Z);
        }

        f32 iZR = 1.f / (maxz - minz);
        for (const gui_instance &gui : Gui::items)
            cache::gui.emplace_back( (gui.Z - minz) * iZR, gui.pos, gui.size, vec4(gui.col, 0) );
        
        glAttribute(gui_VAO)
            .data<GuiData>(gui_IBO, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, cache::gui)
            .vbuffer<vec2>(0, gui_VBO)
            .vbuffer<GuiData>(1, gui_IBO)
            .idraw_array(6, cache::gui.size());
    }
    */

    glfwSwapBuffers(glfw_window);
}

void Perekop::init_render() {
    glAttribute(&meshVAO)
        // vertex (pos, norm, uv)
        .item<vec3, vec3, vec2>()
        // model (t, size, meta)
        .item_instanced<vec4, vec4, vec4, vec4, vec3, vec4>();
    
    vec2 gui_V[6] = {{0,0}, {0,1}, {1,0}, {1,0}, {0,1}, {1,1}};
    glAttribute(&guiVAO)
        .gbuffer<1>(&guiVBO).gbuffer<1>(&guiIBO)
        .item<vec2>()
        .item_instanced<float, vec2, vec2, vec4>()
        .data<vec2>(guiVBO, GL_ARRAY_BUFFER, GL_STATIC_DRAW, gui_V);
    
    string vsrc = file("engine/assets/shaders/vertexgui.glsl");
    guiShader = load_program({
        load_shader({vsrc}, GL_VERTEX_SHADER),
        load_shader({"#version 430\n in vec4 col2; out vec4 fragColor; void main() { fragColor = col2; }"},  GL_FRAGMENT_SHADER)
    });
    
}