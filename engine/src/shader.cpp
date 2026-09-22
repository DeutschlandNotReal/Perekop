#define PK_INTERNAL
#include <PKINT/internal.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <PK/shader.hpp>
#include <PK/file.hpp>
#include <stb_image.h>
using namespace pk;
using std::string_view;
using std::string;

unsigned VAOmesh, VBOmesh;
class VertexArray {
    int index{0};
    template <typename T> void member(int d, int& o) {
        if constexpr (std::is_same_v<T, float> || std::is_same_v<T, quat> ||
            std::is_same_v<T, vec4> || std::is_same_v<T, vec3> || std::is_same_v<T, vec2>)
            glVertexAttribFormat(index, sizeof(T) >> 2, GL_FLOAT, 0, o);
        else if constexpr(std::is_same_v<T, int> || std::is_same_v<T, unsigned>)
            glVertexAttribFormat(index, 1, std::is_same_v<T, int> ? GL_INT : GL_UNSIGNED_INT, 0, o);

        glVertexAttribBinding(index, d);
        glEnableVertexAttribArray(index++);

        o += sizeof(T);
    }

    public:
        VertexArray() = default;
        VertexArray(GLuint array) { glBindVertexArray(array); }
        VertexArray(GLuint* array) { glGenVertexArrays(1, array); glBindVertexArray(*array); }
        template <typename... T> VertexArray& Layout(int binding, int divisor) {
            glVertexBindingDivisor(binding, divisor);
            int offset{0};
            (member<T>(binding, offset), ...);
            return *this;
        }

        VertexArray& BindElements(GLuint b) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b); return *this;}
        template <int L> VertexArray& Buffers(GLuint* b) { glGenBuffers(L, b); return *this;}
        
        template <typename T> VertexArray& Upload(GLuint buffer, GLenum type, GLenum usage, span<T> data) {
            glBindBuffer(type, buffer);
            glBufferData(type, data.size() * sizeof(T), data.begin(), usage);

            return *this;
        }

        template <typename T> VertexArray& VertexBuffer(int binding, GLuint buffer) {
            glBindVertexBuffer(binding, buffer, 0, sizeof(T));
            return *this;
        }

        VertexArray& Draw(int indcount, int usecount) {
            glDrawElementsInstanced(GL_TRIANGLES, indcount, GL_UNSIGNED_INT, 0, usecount);
            return *this;
        }

        VertexArray& DrawArray(int n_vert, int n_users) {
            glDrawArraysInstanced(GL_TRIANGLES, 0, n_vert, n_users);
            return *this;
        }
};

GLuint LoadShader(string_view src, GLenum T) {
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

ShaderStage::ShaderStage(ShaderStage::Stage T, const path& path) {
    string src = ReadFile(path);
    id = LoadShader(src, T);
}

ShaderStage::ShaderStage(ShaderStage::Stage T, string_view src):
    id(LoadShader(src, T))
{}

ShaderStage::~ShaderStage() { glDeleteShader(id); }

Shader::Shader(list<ShaderStage> shaders, list<UniformConfig> u) noexcept {
    program = glCreateProgram();
    for (const auto& shader : shaders)
        glAttachShader(program, shader.id);

    glLinkProgram(program);

    int linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char log[4096];
        glGetProgramInfoLog(program, 4096, nullptr, log);
        printf("\033[31mShader program failed to link:\n%s\n\033[0m", log);
    }

    uniCount = u.size();
    uniData = pk::alloc<Uniform>(u.size());
    for (unsigned i = 0; i < u.size(); i++) {
        auto uni = u.begin()[i];
        uniData[i] = {
            (unsigned short) glGetUniformLocation(program, uni.name.begin()),
            uni.type,
            uni.data
        };
    }
}

void Shader::Apply() const noexcept {
    glUseProgram(program);

    unsigned texture_index{0};

    for (unsigned i = 0; i < uniCount; i++) {
        const Uniform& u = uniData[i];
        using enum Shader::Type;
        switch (u.type) {
            case Mat3:
                glUniformMatrix3fv(u.layout, 1, false, (float*)u.data); break;
            case Mat4:
                glUniformMatrix4fv(u.layout, 1, false, (float*)u.data); break;
            case Float:
                glUniform1f(u.layout, *(float*)u.data); break;
            case Int:
                glUniform1i(u.layout, *(int*)u.data); break;
            case Vec2:
                glUniform2fv(u.layout, 1, (float*)u.data); break;
            case Vec3:
                glUniform3fv(u.layout, 1, (float*)u.data); break;
            case Vec4:
                glUniform4fv(u.layout, 1, (float*)u.data); break;
            case Texture: {
                glActiveTexture(GL_TEXTURE0 + texture_index);
                glBindTexture(GL_TEXTURE_2D, ((pk::Texture*)u.data)->id);
                glUniform1i(u.layout, texture_index);
                texture_index++;
                break;
            }
        }
    }
}

Texture::Texture(const path& path) {
    int channels;

    uint8_t* data = stbi_load(path.string().c_str(), &w, &h, &channels, 0);
    if (!data) return;
    int format = (channels==1) ? GL_RED : (channels==3) ? GL_RGB : GL_RGBA;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(
        GL_TEXTURE_2D, 
        0, 
        format, 
        w, h, 
        0, 
        format, 
        GL_UNSIGNED_BYTE, 
        data
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
};

void Mesh::Load() {
    VertexArray(VAOmesh)
        .Buffers<3>(&VBO)
        .Upload<decltype(Mesh::indices)::type>(EBO, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices)
        .Upload<decltype(Mesh::vertices)>(VBO, GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices)
        .VertexBuffer<Mesh::Vertex>(0, VBO);
}

void Mesh::Unload() {
    glDeleteBuffers(3, &VBO);
    VBO = EBO = IBO = 0;
}

void Perekop::RenderBegin() noexcept {
    VertexArray(&VAOmesh)
        // vertex (pos, norm, uv)
        .Layout<vec3, vec3, vec2>(0, 0)
        // model (t, size, meta)
        .Layout<vec4, vec4, vec4, vec4, vec3, vec4>(1, 1);
}

struct ShaderModel {
    mat4 matrix; vec3 scale; vec4 metadata; 
};

void Render::Draw(const Shader& prog, const Mesh& mesh, span<Model> models) const noexcept {
    if (!mesh.Loaded() || !models.size()) return;

    vector<ShaderModel> instances;
    instances.reserve(models.size());
    for (const Model& model : models) instances.emplace(
        model.pose, model.scale, model.metadata
    );

    prog.Apply();

    VertexArray(VAOmesh)
        .BindElements(mesh.EBO)
        .Upload<ShaderModel>(mesh.IBO, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, instances)
        .VertexBuffer<ShaderModel>(1, mesh.IBO)
        .Draw(mesh.indices.size(), instances.size());
}

void Render::Draw(const Shader& prog) const noexcept {
    glDisable(GL_DEPTH_TEST);
    prog.Apply();
    VertexArray(VAOmesh).DrawArray(3, 1);
    glEnable(GL_DEPTH_TEST);
}

void Render::Swap() const noexcept {
    glfwSwapBuffers(Perekop::glfw_window);
}

void Render::Fill(vec3 col) const noexcept {
    glClearColor(col.x, col.y, col.z, 1.0);
}

void Render::Clear(int buffers) const noexcept {
    glClear(buffers);
}

void Render::Target(const Framebuffer& fb, Render::DrawTarget target) const noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);
    Viewport(fb.w, fb.h);
    glDrawBuffer(fb.color_attached ? GL_COLOR_ATTACHMENT0 : target);
}

void Framebuffer::AttachDepth(const Texture& t) noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        t.id,
        0
    );

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer is incomplete: 0x%x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
}

void Framebuffer::AttachColor(const Texture& t) noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        t.id,
        0
    );
    color_attached = true;
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

Texture Texture::Color(int x, int y) noexcept {
    Texture texture;
    texture.w = x;
    texture.h = y;
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        x, y,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return texture;
}

Texture Texture::Depth(int x, int y, bool compare) noexcept {
    Texture texture;
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT24,
        x, y,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    if (compare) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    }

    return texture;
}

void Render::Viewport() const noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);
    int x, y; 
    glfwGetWindowSize(Perekop::glfw_window, &x, &y);
    glViewport(0, 0, x, y);
}

void Render::Viewport(int x, int y) const noexcept {
    glViewport(0, 0, x, y);
}

void Render::Viewport(int x, int y, int w, int h) const noexcept {
    glViewport(x, y, w, h);
}

Framebuffer::Framebuffer(int x, int y) noexcept {
    w = x; h = y;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}