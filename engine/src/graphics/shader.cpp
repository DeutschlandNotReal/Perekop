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

GLuint load_shader(string_view src, GLenum T) {
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

Shader::Shader(Shader::type T, const path& path) {
    string src = file(path);
    id = load_shader(src, T);
}

Shader::Shader(Shader::type T, string_view src):
    id(load_shader(src, T))
{}

Shader::~Shader() { glDeleteShader(id); }

ShaderProgram::ShaderProgram(const Shader& v, const Shader& f, std::initializer_list<UniformConfig> u) noexcept {
    program = glCreateProgram();
    glAttachShader(program, v.id);
    glAttachShader(program, f.id);
    glLinkProgram(program);

    uniform_n = u.size();
    uniform = pk::alloc<Uniform>(u.size());
    for (unsigned i = 0; i < u.size(); i++) {
        auto uni = u.begin()[i];
        uniform[i] = {
            (unsigned short) glGetUniformLocation(program, uni.name.begin()),
            uni.type,
            uni.data
        };
    }
}

void ShaderProgram::apply() const noexcept {
    glUseProgram(program);

    for (unsigned i = 0; i < uniform_n; i++) {
        const Uniform& u = uniform[i];
        using enum ShaderProgram::UDataType;
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
            case texture: 
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, ((Texture*)u.data)->id);
                glUniform1i(u.layout, 0);
        }
    }
}

Texture::Texture(const path& path) {
    int width, height, channels;

    uint8_t* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
    if (!data) return;

    int format = (channels==1) ? GL_RED : (channels==3) ? GL_RGB : GL_RGBA;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
};

void Mesh::load() {
    glAttribute(VAOmesh)
        .gbuffer<3>(&VBO)
        .data<uint16_t>(EBO, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices)
        .data<Mesh::Vertex>(VBO, GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices)
        .vbuffer<Mesh::Vertex>(0, VBO);
}

void Mesh::unload() {
    glDeleteBuffers(3, &VBO);
    VBO = EBO = IBO = 0;
}

void Perekop::init_render() {
    glAttribute(&VAOmesh)
        // vertex (pos, norm, uv)
        .item<vec3, vec3, vec2>()
        // model (t, size, meta)
        .item_instanced<vec4, vec4, vec4, vec4, vec3, vec4>();   
}

struct ShaderModel { mat4 matrix; vec3 scale; vec4 metadata; };
ShaderModel* shader_cache = pk::alloc<ShaderModel>(2048); 
unsigned shader_cache_size = 2048;

void ShaderProgram::draw(const Mesh& mesh, span<Model> models) const noexcept {
    if (!mesh.loaded() || !models.size()) return;
    apply();
    if (models.size() > shader_cache_size) {
        pk::free(shader_cache);
        shader_cache = pk::alloc<ShaderModel>(models.size());
        shader_cache_size = models.size();
    }
    
    for (unsigned i = 0; i < models.size(); i++) {
        const Model& model = models[i];
        shader_cache[i] = {model.pose, model.scale, model.metadata};
    }

    glAttribute(VAOmesh)
        .bind_elements(mesh.EBO)
        .data<ShaderModel>(mesh.IBO, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, shader_cache)
        .vbuffer<ShaderModel>(1, mesh.IBO)
        .idraw(mesh.indices.size(), models.size());
}

void Perekop::Window::swap_buffers() noexcept {
    glfwSwapBuffers(glfw_window);
}

void Perekop::Window::clear(vec3 col) noexcept {
    glClearColor(col.x, col.y, col.z, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
