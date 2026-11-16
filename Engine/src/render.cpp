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

// VertexARr
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

void VertexArr::bindBuffer(unsigned Binding, Buffer glBuffer, BufferTar target, size_t stride) noexcept {
    if (target == BufferTar::element) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glBuffer.getIndex());
        return;
    }

    glBindVertexBuffer(Binding, glBuffer.getIndex(), 0, (GLsizei)stride);
}

// Buffer
void Buffer::generate() noexcept {
    if (index) { warn("attempted to generate buffer twice"); return; }
    glGenBuffers(1, &index);
}

void Buffer::upload(BufferTar target, BufferUse usage, span<> data) noexcept {
    glBindBuffer((GLenum)target, index);
    glBufferData((GLenum)target, (GLsizeiptr)data.size(), data.begin(), (GLenum)usage);
}

void Buffer::destroy() noexcept {
    if (!index) return;
    glDeleteBuffers(1, &index);
    index = 0;
}

// ShaderStage
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
        warn(std::format("Shader stage failed to compile: \n{}", log));
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
        warn(std::format("Shader failed to compile: \n{}", log));
    }
}

// Shader
Shader::Shader(std::initializer_list<shaderStage> stages) noexcept {
    index = glCreateProgram();

    for (const shaderStage& stage : stages)
        glAttachShader(index, stage.getIndex());

    glLinkProgram(index);

    int success;
    glGetProgramiv(index, GL_LINK_STATUS, &success);

    if (!success) {
        char log[4096];
        glGetProgramInfoLog(index, 4096, nullptr, log);
        warn(std::format("Shader failed to link: \n", log));
    }
}

void Shader::use() const noexcept {
    glUseProgram(index);
}

// Shader Uniforms
Shader::uniform::uniform(uint prog, std::string_view name, std::source_location loc) noexcept: program(prog) {
    index = (uint) glGetUniformLocation(prog, name.data());

    if (index == ~0u) warn(std::format("Cannot find uniform {}", name), loc);
};

int Shader::uniform::operator=(int value) noexcept {
    glUseProgram(program);
    glUniform1i((GLint)index, value);
    return value;
}

float Shader::uniform::operator=(float value) noexcept {
    glUseProgram(program);
    glUniform1f((GLint)index, value);
    return value;
}

vec2 Shader::uniform::operator=(vec2 value) noexcept {
    glUseProgram(program);
    glUniform2fv((GLint)index, 1, (float*)&value);
    return value;
}

vec3 Shader::uniform::operator=(vec3 value) noexcept {
    glUseProgram(program);
    glUniform3fv((GLint)index, 1, (float*)&value);
    return value;
}

vec4 Shader::uniform::operator=(vec4 value) noexcept {
    glUseProgram(program);
    glUniform4fv((GLint)index, 1, (float*)&value);
    return value;
}

mat3 Shader::uniform::operator=(mat3 value) noexcept {
    glUseProgram(program);
    glUniformMatrix3fv((GLint)index, 1, GL_FALSE, (float*)&value);
    return value;
}

mat4 Shader::uniform::operator=(mat4 value) noexcept {
    glUseProgram(program);
    glUniformMatrix4fv((GLint)index, 1, GL_FALSE, (float*)&value);
    return value;
}

Shader::uniform Shader::getUniform(std::string_view name, std::source_location loc) const noexcept {
    return {index, name, loc};
}

void Shader::setTexture(std::string_view name, unsigned slot, const Texture& Texture) noexcept {
    glUseProgram(index);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, Texture.getIndex());
    glUniform1i(glGetUniformLocation(index, name.data()), (GLint)slot);
}

// texture
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

void Texture::allocate(
    int width,
    int height,
    uint internalFormat
) {
    w = width;
    h = height;

    glBindTexture(GL_TEXTURE_2D, index);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internalFormat,
        w,
        h,
        0,
        GL_RGBA,
        GL_FLOAT,
        nullptr
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE
    );
}

// renderer
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
    auto size = Window::main.size();
  
    glViewport(0, 0, size.x, size.y);
}

void Renderer::swap() noexcept {
    glfwSwapBuffers(Window::main);
}

// framebuffer
void Framebuffer::generate() {
    if (index) { warn("attempted to generate framebuffer twice"); return; }

    glGenFramebuffers(1, &index);
}

void Framebuffer::bind() const noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, index);
}

void Framebuffer::unbind() noexcept {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void Framebuffer::destroy() {
    if (!index) return;

    glDeleteFramebuffers(1, &index);
    index = 0;
}

// renderbuffer
void Renderbuffer::generate() {
    if (index) { warn("attempted to generate renderbuffer twice"); return; }

    glGenRenderbuffers(1, &index);
}

pk::Shader boxshader;
pk::Buffer boxIBO;
pk::VertexArr boxarr;

void Renderer::drawWireboxes(const span<Wirebox> boxes) noexcept {
    if (boxes.size() == 0 || !Window::main.camera) return;

    Camera& cam = *Window::main.camera;

    boxshader.use();
    boxshader.getUniform("camera") = cam.projection() * cam.view();

    boxarr.bind();
    boxIBO.upload(BufferTar::array, BufferUse::dynamic, boxes);
    boxarr.bindBuffer(1, boxIBO, BufferTar::array, sizeof(Wirebox));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    drawArraysInstanced(36, boxes.size(), PrimMode::trigs);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

#include <PK/world.hpp>
void Perekop::init::render() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    boxshader = {
        shaderStage(shaderType::Vertex, world::assets/"shaders/cubevert.glsl"),
        shaderStage(shaderType::fragment, world::assets/"shaders/cubefrag.glsl")
    };

    boxIBO.generate();
    boxarr = VertexArr::generate();
    boxarr.bind();
    VertexArr::Builder().setBD(1, 1).mat(3).vec(3).vec(3).vec(4);
}
