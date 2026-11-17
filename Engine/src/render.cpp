#include "./internal.hpp"

#include <PK/Render/Renderer.hpp>
#include <PK/Render/window.hpp>
#include <PK/Render/Resources/framebuffer.hpp>
#include <PK/Util/file.hpp>
#include <PK/debug.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

using std::string;
using namespace pk;

typedef void (*GLGenT)(int, unsigned int*);
typedef void (*GLDelT)(int, const unsigned int*);

struct glSetter { GLGenT* gen{nullptr}; GLDelT* del{nullptr}; };
template <typename T> glSetter setter;

template<> glSetter setter<VertexArr>   {&glGenVertexArrays, &glDeleteVertexArrays};
template<> glSetter setter<Buffer>      {&glGenBuffers, &glDeleteBuffers};
template<> glSetter setter<Texture>     {&glGenTextures, &glDeleteTextures}; 
template<> glSetter setter<Framebuffer> {&glGenFramebuffers, &glDeleteFramebuffers};
template<> glSetter setter<Renderbuffer>{&glGenRenderbuffers, &glDeleteRenderbuffers};

template <typename T> T glResource<T>::create() {
    T item;
    (*setter<T>.gen)(1, &item.index);
    return item;
}

template <typename T> glResource<T>::~glResource() {
    if (index) (*setter<T>.del)(1, &index);
    index = 0;
}

template <typename T> glResource<T>::glResource(glResource&& from) noexcept {
    exchange(index, from.index, 0);
}

template <typename T> glResource<T>& glResource<T>::operator=(glResource&& from) noexcept {
    if (index) (*setter<T>.del)(1, &index);
    exchange(index, from.index, 0);
    return *this;
}

VertexArr::Builder& VertexArr::Builder::setBD(uint bind, uint div) noexcept {
    binding = bind;
    offset = 0;
    glVertexBindingDivisor(bind, div);
    return *this;
}

namespace {
    void SetupAttribute(uint &index, ullong &offset, uint bind, uint size) noexcept {
        glVertexAttribBinding(index, bind);
        glEnableVertexAttribArray(index++);
        offset += size;
    }
}

VertexArr::Builder& VertexArr::Builder::vec(int n) noexcept {
    glVertexAttribFormat(index, n, GL_FLOAT, GL_FALSE, offset);
    SetupAttribute(index, offset, binding, sizeof(float) * n);
    return *this;
}

VertexArr::Builder& VertexArr::Builder::uvec(int n) noexcept {
    glVertexAttribIFormat(index, n, GL_UNSIGNED_INT, offset);
    SetupAttribute(index, offset, binding, sizeof(uint) * n);
    return *this;
}

VertexArr::Builder& VertexArr::Builder::ivec(int n) noexcept {
    glVertexAttribIFormat(index, n, GL_INT, offset);
    SetupAttribute(index, offset, binding, sizeof(int) * n);
    return *this;
}

VertexArr::Builder& VertexArr::Builder::mat(int n) noexcept {
    for (int i{0}; i<n; i++) this->vec(n);

    return *this;
}


void VertexArr::bind() noexcept { glBindVertexArray(index); }

void VertexArr::bindBuffer(uint bind, const Buffer& buf, BufferTar target, size_t stride) noexcept {
    switch (target) {
        case BufferTar::element: 
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.getIndex());
            break;
    
        case BufferTar::array:
            glBindVertexBuffer(bind, buf.getIndex(), 0, (GLsizei) stride);
            break;
    }
}

void Buffer::upload(BufferTar target, BufferUse usage, span<> data) noexcept {
    glBindBuffer((GLenum) target, index);
    glBufferData((GLenum) target, data.size(), data.begin(), (GLenum) usage);
}

namespace {
    void compileShaderStage(uint& index, std::string_view src, ShaderType type) {
        index = glCreateShader((GLenum) type);

        const char* data{src.data()}; int size{(int) src.length()};

        glShaderSource(index, 1, &data, &size);
        glCompileShader(index);

        int ok;
        glGetShaderiv(index, GL_COMPILE_STATUS, &ok);

        if (!ok) {
            char log[4096];
            glGetShaderInfoLog(index, sizeof(log), nullptr, log);

            warn(std::format("{} shader stage failed to compile: \n{}", type == ShaderType::vertex ? "Vertex" : "Fragment", log));
        }
    }
}

ShaderStage::ShaderStage(ShaderType type, const path& path) noexcept {
    string src = readFile(path);

    compileShaderStage(index, src, type);
}

ShaderStage::ShaderStage(ShaderType type, std::string_view src) noexcept {
    compileShaderStage(index, src, type);
}

Shader::Shader(std::initializer_list<ShaderStage> stages) noexcept {
    index = glCreateProgram();

    for (const ShaderStage& stage : stages)
        glAttachShader(index, stage.getIndex());

    glLinkProgram(index);

    int success;
    glGetProgramiv(index, GL_LINK_STATUS, &success);

    if (!success) {
        char log[4096];
        glGetProgramInfoLog(index, 4096, nullptr, log);
        warn(std::format("Shader failed to link: \n{}", log));
    }
}

void Shader::use() const noexcept {
    glUseProgram(index);
}

Shader::uniform::uniform(uint prog, std::string_view name, std::source_location loc) noexcept: program(prog) {
    index = (uint) glGetUniformLocation(prog, name.data());

    if (index == ~0u) warn(std::format("Cannot find uniform {}", name), loc);
};

int Shader::uniform::operator=(int val) noexcept {
    glUniform1i(index, val); return val;
}

float Shader::uniform::operator=(float val) noexcept {
    glUniform1f(index, val); return val;
}

vec2 Shader::uniform::operator=(vec2 val) noexcept {
    glUniform2fv(index, 1, &val.x); return val;
}

vec3 Shader::uniform::operator=(vec3 val) noexcept {
    glUniform3fv(index, 1, &val.x); return val;
}

vec4 Shader::uniform::operator=(vec4 val) noexcept {
    glUniform4fv(index, 1, &val.x); return val;
}

mat3 Shader::uniform::operator=(mat3 val) noexcept {
    glUniformMatrix3fv(index, 1, GL_FALSE, &val[0][0]); return val;
}

mat4 Shader::uniform::operator=(mat4 val) noexcept {
    glUniformMatrix4fv(index, 1, GL_FALSE, &val[0][0]); return val;
}

Shader::uniform Shader::getUniform(std::string_view name, std::source_location loc) const noexcept {
    return {index, name, loc};
}

void Shader::setTexture(std::string_view name, unsigned slot, glRefference<Texture> texture) noexcept {
    glUseProgram(index);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture.getIndex());
    glUniform1i(glGetUniformLocation(index, name.data()), (GLint)slot);
}

Texture::Texture(const path& path) noexcept {
    int channels;
    uchar* data = stbi_load(path.string().c_str(), &w, &h, &channels, 0);

    if (!data) {
        warn(std::format("No texture file {} found", path.string()));
        return;
    }

    int format = channels == 1 ? GL_RED : channels == 3 ? GL_RGB : GL_RGBA;

    glGenTextures(1, &index);
    glBindTexture(GL_TEXTURE_2D, index);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

void Texture::allocate(int width, int height, uint format) {
    w = width;
    h = height;

    glBindTexture(GL_TEXTURE_2D, index);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        w,
        h,
        0,
        GL_RGBA,
        GL_FLOAT,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Renderer::draw(DrawMode dmode, int n, PrimMode pmode) noexcept {
    switch (dmode) {
        case DrawMode::element: glDrawElements(
            (GLenum) pmode, n, GL_UNSIGNED_INT, nullptr
        ); break;

        case DrawMode::array: glDrawArrays(
            (GLenum) pmode, 0, n
        ); break;
    }
}

void Renderer::drawInstanced(DrawMode dmode, int n, int entities, PrimMode pmode) noexcept {
    switch (dmode) {
        case DrawMode::element: glDrawElementsInstanced(
            (GLenum) pmode, n, GL_UNSIGNED_INT, nullptr, entities
        ); break;

        case DrawMode::array: glDrawArraysInstanced(
            (GLenum) pmode, 0, n, entities
        ); break;
    }
}

void Renderer::setMode(RasterMode mode) noexcept { glPolygonMode(GL_FRONT_AND_BACK, (GLenum)mode); }
void Renderer::fill(vec3 colour) noexcept { glClearColor(colour.x, colour.y, colour.z, 1.0f); }
void Renderer::clear(int flags) noexcept { glClear((GLbitfield)flags); }
void Renderer::swap() noexcept { glfwSwapBuffers(Window::main); }

void Renderer::viewport() noexcept {
    glDrawBuffer(GL_BACK);
    auto size = Window::main.size();
  
    glViewport(0, 0, size.x, size.y);
}

void Framebuffer::bind() const noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, index);
}

void Framebuffer::attach(Texture& texture, uint slot) {
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + slot,
        GL_TEXTURE_2D,
        texture.getIndex(),
        0
    );
}

void Framebuffer::attach(Renderbuffer& rb) {
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        rb.getIndex()
    );
}

bool Framebuffer::complete() const noexcept {
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void Perekop::initRender() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    Mesh::VAO = VertexArr::create();
    Mesh::VAO.config()
        .vec(3) // pos
        .vec(3) // nor
        .vec(2) // uv
    ();
}

template class pk::glResource<VertexArr>;
template class pk::glResource<Buffer>;
template class pk::glResource<Texture>;
template class pk::glResource<Framebuffer>;
template class pk::glResource<Renderbuffer>;