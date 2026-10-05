#define PK_INTERNAL
#include <common.hpp>
#include <PK/Render/renderer.hpp>
#include <PK/Render/mesh.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <PK/Util/file.hpp>
using std::string;
using namespace pk::Render;

void Perekop::RenderBegin() noexcept {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

VertexArray::Builder&
VertexArray::Builder::SetBD(unsigned Binding, unsigned Divisor) noexcept {
    binding = Binding;
    offset = 0;
    glVertexBindingDivisor(Binding, Divisor);
    return *this;
}

VertexArray::Builder&
VertexArray::Builder::Float(int n) noexcept {
    glVertexAttribFormat(index, n, GL_FLOAT, GL_FALSE, offset);
    glVertexAttribBinding(index, binding);
    glEnableVertexAttribArray(index);

    ++index;
    offset += sizeof(float) * n;

    return *this;
}

VertexArray::Builder&
VertexArray::Builder::Uint() noexcept {
    glVertexAttribIFormat(index, 1, GL_UNSIGNED_INT, offset);
    glVertexAttribBinding(index, binding);
    glEnableVertexAttribArray(index);

    ++index;
    offset += sizeof(unsigned);

    return *this;
}

VertexArray::Builder&
VertexArray::Builder::Int() noexcept {
    glVertexAttribIFormat(index, 1, GL_INT, offset);
    glVertexAttribBinding(index, binding);
    glEnableVertexAttribArray(index);

    ++index;
    offset += sizeof(int);

    return *this;
}

VertexArray::Builder&
VertexArray::Builder::Matrix4() noexcept {
    Float(4); Float(4); Float(4);Float(4);
    return *this;
}

VertexArray VertexArray::generate() noexcept {
    VertexArray array;
    glGenVertexArrays(1, &array.index);
    return array;
}

void VertexArray::Bind() noexcept {
    glBindVertexArray(index);
}

void VertexArray::BindBuffer(unsigned Binding, const Buffer& buffer, BufferTarget Target, size_t stride) noexcept {
    if (Target == BufferTarget::Element) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.index);
        return;
    }

    glBindVertexBuffer(Binding, buffer.index, 0, (GLsizei)stride);
}

Buffer Buffer::Generate() noexcept {
    Buffer buffer;
    glGenBuffers(1, &buffer.index);
    return buffer;
}

void Buffer::Upload(BufferTarget target, BufferUsage usage, span<void> data) noexcept {
    glBindBuffer((GLenum)target, index);
    glBufferData((GLenum)target, (GLsizeiptr)data.size(), data.begin(), (GLenum)usage);
}

void Buffer::Delete() noexcept {
    if (!index) return;
    glDeleteBuffers(1, &index);
    index = 0;
}

Buffer::Buffer(Buffer&& buffer) noexcept:
    index(std::exchange(buffer.index, 0))
{}

Buffer& Buffer::operator=(Buffer&& buffer) noexcept {
    if (this == &buffer) return *this;
    Delete();
    index = std::exchange(buffer.index, 0);
    return *this;
}

ShaderStage::ShaderStage(ShaderType type, const path& path) noexcept {
    string source = ReadFile(path);

    index = glCreateShader((GLenum)type);

    const char* data = source.data();
    int size = (int)source.size();

    glShaderSource(index, 1, &data, &size);
    glCompileShader(index);

    int success;
    glGetShaderiv(index, GL_COMPILE_STATUS, &success);

    if (!success) {
        char log[4096];
        glGetShaderInfoLog(index, 4096, nullptr, log);
        printf("\033[31mShader failed to compile:\n%s\n\033[0m", log);
    }
}

ShaderStage::ShaderStage(ShaderType type, std::string_view source) noexcept {
    index = glCreateShader((GLenum)type);

    const char* data = source.data();
    int size = (int)source.size();

    glShaderSource(index, 1, &data, &size);
    glCompileShader(index);

    int success;
    glGetShaderiv(index, GL_COMPILE_STATUS, &success);

    if (!success) {
        char log[4096];
        glGetShaderInfoLog(index, 4096, nullptr, log);
        printf("\033[31mShader failed to compile:\n%s\n\033[0m", log);
    }
}

Shader::Shader(std::initializer_list<ShaderStage> stages) noexcept {
    index = glCreateProgram();

    for (const ShaderStage& stage : stages)
        glAttachShader(index, stage.GetIndex());

    glLinkProgram(index);

    int success;
    glGetProgramiv(index, GL_LINK_STATUS, &success);

    if (!success) {
        char log[4096];
        glGetProgramInfoLog(index, 4096, nullptr, log);
        printf("\033[31mShader program failed to link:\n%s\n\033[0m", log);
    }
}

void Shader::Use() const noexcept {
    glUseProgram(index);
}

Shader::Uniform::Uniform(unsigned Program, std::string_view name) noexcept:
    program(Program),
    index((unsigned)glGetUniformLocation(Program, name.data()))
{}

int Shader::Uniform::operator=(int value) noexcept {
    glUseProgram(program);
    glUniform1i((GLint)index, value);
    return value;
}

float Shader::Uniform::operator=(float value) noexcept {
    glUseProgram(program);
    glUniform1f((GLint)index, value);
    return value;
}

vec2 Shader::Uniform::operator=(vec2 value) noexcept {
    glUseProgram(program);
    glUniform2fv((GLint)index, 1, (float*)&value);
    return value;
}

vec3 Shader::Uniform::operator=(vec3 value) noexcept {
    glUseProgram(program);
    glUniform3fv((GLint)index, 1, (float*)&value);
    return value;
}

vec4 Shader::Uniform::operator=(vec4 value) noexcept {
    glUseProgram(program);
    glUniform4fv((GLint)index, 1, (float*)&value);
    return value;
}

mat3 Shader::Uniform::operator=(mat3 value) noexcept {
    glUseProgram(program);
    glUniformMatrix3fv((GLint)index, 1, GL_FALSE, (float*)&value);
    return value;
}

mat4 Shader::Uniform::operator=(mat4 value) noexcept {
    glUseProgram(program);
    glUniformMatrix4fv((GLint)index, 1, GL_FALSE, (float*)&value);
    return value;
}

Shader::Uniform Shader::GetUniform(std::string_view name) const noexcept {
    return Uniform(index, name);
}

void Shader::SetTexture(std::string_view name, unsigned slot, const Texture& texture) noexcept {
    glUseProgram(index);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture.index);
    glUniform1i(glGetUniformLocation(index, name.data()), (GLint)slot);
}

Texture::Texture(const path& path) noexcept {
    int channels;
    unsigned char* data = stbi_load(path.string().c_str(), &w, &h, &channels, 0);

    if (!data)
        return;

    int format = channels == 1 ? GL_RED : channels == 3 ? GL_RGB : GL_RGBA;

    glGenTextures(1, &index);
    glBindTexture(GL_TEXTURE_2D, index);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

Texture Texture::Colour(int width, int height) noexcept {
    Texture texture;

    texture.w = width;
    texture.h = height;

    glGenTextures(1, &texture.index);
    glBindTexture(GL_TEXTURE_2D, texture.index);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return texture;
}

Texture Texture::Depth(int width, int height, bool compare) noexcept {
    Texture texture;

    texture.w = width;
    texture.h = height;

    glGenTextures(1, &texture.index);
    glBindTexture(GL_TEXTURE_2D, texture.index);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

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

Framebuffer::Framebuffer(int width, int height) noexcept:
    w(width), h(height)
{
    glGenFramebuffers(1, &index);
}

void Framebuffer::Bind() noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, index);
}

void Framebuffer::AttachColour(const Texture& texture) noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, index);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.index, 0);

    ColourAttached = true;
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

void Framebuffer::AttachDepth(const Texture& texture) noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, index);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.index, 0);

    if (!ColourAttached) {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("Framebuffer is incomplete: 0x%x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
}

void Renderer::DrawElems(int indices, PrimitiveMode mode) noexcept {
    glDrawElements((GLenum)mode, indices, GL_UNSIGNED_INT, nullptr);
}

void Renderer::DrawArrays(int vertices, PrimitiveMode mode) noexcept {
    glDrawArrays((GLenum)mode, 0, vertices);
}

void Renderer::iDrawElems(int indices, int entities, PrimitiveMode mode) noexcept {
    glDrawElementsInstanced((GLenum)mode, indices, GL_UNSIGNED_INT, nullptr, entities);
}

void Renderer::iDrawArrays(int vertices, int entities, PrimitiveMode mode) noexcept {
    glDrawArraysInstanced((GLenum)mode, 0, vertices, entities);
}

void Renderer::SetMode(RasterMode mode) noexcept {
    glPolygonMode(GL_FRONT_AND_BACK, (GLenum)mode);
}

void Renderer::Target(const Framebuffer& framebuffer, DrawTarget target) noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.index);
    Viewport(framebuffer.w, framebuffer.h);

    if (framebuffer.ColourAttached)
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    else
        glDrawBuffer((GLenum)target);
}

void Renderer::Fill(vec3 colour) noexcept {
    glClearColor(colour.x, colour.y, colour.z, 1.0f);
}

void Renderer::Clear(int flags) noexcept {
    glClear((GLbitfield)flags);
}

void Renderer::Viewport() noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    int width, height;
    glfwGetWindowSize(Perekop::glfw_window, &width, &height);

    glViewport(0, 0, width, height);
}

void Renderer::Viewport(int width, int height) noexcept {
    glViewport(0, 0, width, height);
}

void Renderer::Viewport(int x, int y, int width, int height) noexcept {
    glViewport(x, y, width, height);
}

void Renderer::Swap() noexcept {
    glfwSwapBuffers(Perekop::glfw_window);
}