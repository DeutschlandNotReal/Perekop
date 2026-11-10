#include "./internal.hpp"

#include <PK/Render/Renderer.hpp>
#include <PK/Render/window.hpp>
#include <PK/Util/file.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

using std::string;
using namespace pk;

void Perekop::init::render() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

VertexArr::Builder&
VertexArr::Builder::setBD(uint bind, uint div) noexcept {
    binding = bind;
    offset = 0;
    glVertexBindingDivisor(bind, div);
    return *this;
}

VertexArr::Builder&
VertexArr::Builder::vec(int n) noexcept {
    glVertexAttribFormat(index, n, GL_FLOAT, GL_FALSE, offset);
    glVertexAttribBinding(index, binding);
    glEnableVertexAttribArray(index);

    ++index;
    offset += sizeof(float) * n;

    return *this;
}

VertexArr::Builder&
VertexArr::Builder::uvec(int n) noexcept {
    glVertexAttribIFormat(index, n, GL_UNSIGNED_INT, offset);
    glVertexAttribBinding(index, binding);
    glEnableVertexAttribArray(index);

    ++index;
    offset += sizeof(uint) * n;

    return *this;
}

VertexArr::Builder&
VertexArr::Builder::ivec(int n) noexcept {
    glVertexAttribIFormat(index, n, GL_INT, offset);
    glVertexAttribBinding(index, binding);
    glEnableVertexAttribArray(index);

    ++index;
    offset += sizeof(int) * n;

    return *this;
}

VertexArr::Builder&
VertexArr::Builder::mat(int n) noexcept {
    for (int i{0}; i<n; i++) this->vec(n);

    return *this;
}

VertexArr VertexArr::generate() noexcept {
    VertexArr array;
    glGenVertexArrays(1, &array.index);
    return array;
}

void VertexArr::bind() noexcept {
    glBindVertexArray(index);
}

void VertexArr::bindBuffer(unsigned Binding, const buffer& buffer, bufferTar target, size_t stride) noexcept {
    if (target == bufferTar::element) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.index);
        return;
    }

    glBindVertexBuffer(Binding, buffer.index, 0, (GLsizei)stride);
}

buffer buffer::generate() noexcept {
    buffer buffer;
    glGenBuffers(1, &buffer.index);
    return buffer;
}

void buffer::upload(bufferTar target, bufferUse usage, span<> data) noexcept {
    glBindBuffer((GLenum)target, index);
    glBufferData((GLenum)target, (GLsizeiptr)data.size(), data.begin(), (GLenum)usage);
}

void buffer::destroy() noexcept {
    if (!index) return;
    glDeleteBuffers(1, &index);
    index = 0;
}

buffer::buffer(buffer&& buffer) noexcept:
    index(std::exchange(buffer.index, 0))
{}

buffer& buffer::operator=(buffer&& buffer) noexcept {
    if (this == &buffer) return *this;
    destroy();
    index = std::exchange(buffer.index, 0);
    return *this;
}

shaderStage::shaderStage(shaderType type, const path& path) noexcept {
    string source = readFile(path);

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

shaderStage::shaderStage(shaderType type, std::string_view source) noexcept {
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

shader::shader(std::initializer_list<shaderStage> stages) noexcept {
    index = glCreateProgram();

    for (const shaderStage& stage : stages)
        glAttachShader(index, stage.getIndex());

    glLinkProgram(index);

    int success;
    glGetProgramiv(index, GL_LINK_STATUS, &success);

    if (!success) {
        char log[4096];
        glGetProgramInfoLog(index, 4096, nullptr, log);
        printf("\033[31mShader program failed to link:\n%s\n\033[0m", log);
    }
}

void shader::use() const noexcept {
    glUseProgram(index);
}

shader::uniform::uniform(unsigned Program, std::string_view name) noexcept:
    program(Program),
    index((unsigned)glGetUniformLocation(Program, name.data()))
{}

int shader::uniform::operator=(int value) noexcept {
    glUseProgram(program);
    glUniform1i((GLint)index, value);
    return value;
}

float shader::uniform::operator=(float value) noexcept {
    glUseProgram(program);
    glUniform1f((GLint)index, value);
    return value;
}

vec2 shader::uniform::operator=(vec2 value) noexcept {
    glUseProgram(program);
    glUniform2fv((GLint)index, 1, (float*)&value);
    return value;
}

vec3 shader::uniform::operator=(vec3 value) noexcept {
    glUseProgram(program);
    glUniform3fv((GLint)index, 1, (float*)&value);
    return value;
}

vec4 shader::uniform::operator=(vec4 value) noexcept {
    glUseProgram(program);
    glUniform4fv((GLint)index, 1, (float*)&value);
    return value;
}

mat3 shader::uniform::operator=(mat3 value) noexcept {
    glUseProgram(program);
    glUniformMatrix3fv((GLint)index, 1, GL_FALSE, (float*)&value);
    return value;
}

mat4 shader::uniform::operator=(mat4 value) noexcept {
    glUseProgram(program);
    glUniformMatrix4fv((GLint)index, 1, GL_FALSE, (float*)&value);
    return value;
}

shader::uniform shader::getUniform(std::string_view name) const noexcept {
    return {index, name};
}

void shader::setTexture(std::string_view name, unsigned slot, const Texture& Texture) noexcept {
    glUseProgram(index);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, Texture.index);
    glUniform1i(glGetUniformLocation(index, name.data()), (GLint)slot);
}

Texture::Texture(const path& path) noexcept {
    int channels;
    uchar* data = stbi_load(path.string().c_str(), &w, &h, &channels, 0);

    if (!data) return;

    int format = channels == 1 ? GL_RED : channels == 3 ? GL_RGB : GL_RGBA;

    glGenTextures(1, &index);
    glBindTexture(GL_TEXTURE_2D, index);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

void Renderer::drawElems(int indices, PrimMode mode) noexcept {
    glDrawElements((GLenum)mode, indices, GL_UNSIGNED_INT, nullptr);
}

void Renderer::drawArrays(int vertices, PrimMode mode) noexcept {
    glDrawArrays((GLenum)mode, 0, vertices);
}

void Renderer::drawElemsInstanced(int indices, int entities, PrimMode mode) noexcept {
    glDrawElementsInstanced((GLenum)mode, indices, GL_UNSIGNED_INT, nullptr, entities);
}

void Renderer::drawArraysInstanced(int vertices, int entities, PrimMode mode) noexcept {
    glDrawArraysInstanced((GLenum)mode, 0, vertices, entities);
}

void Renderer::setMode(RasterMode mode) noexcept {
    glPolygonMode(GL_FRONT_AND_BACK, (GLenum)mode);
}

void Renderer::fill(vec3 colour) noexcept {
    glClearColor(colour.x, colour.y, colour.z, 1.0f);
}

void Renderer::clear(int flags) noexcept {
    glClear((GLbitfield)flags);
}

void Renderer::viewport() noexcept {
    glDrawBuffer(GL_BACK);

    int width, height;
    auto size = mainWindow.size();
    width = static_cast<int>(size.x);
    height = static_cast<int>(size.y);

    glViewport(0, 0, width, height);
}

void Renderer::swap() noexcept {
    mainWindow.swapBuffers();
}