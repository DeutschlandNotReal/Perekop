#pragma once
#include <PK/model.hpp>
#include <PK/mesh.hpp>
#include <string_view>
#include <filesystem>
#include <initializer_list>

namespace pk {
    using path = std::filesystem::path;
    template <typename T> using list = std::initializer_list<T>;

    class Render;
    class Framebuffer;

    enum BufferType : unsigned {
        ColorBuffer = 0x4000,
        DepthBuffer = 0x0100,
    };

    struct ShaderStage {
        enum Stage { Vertex = 0x8B31, Fragment = 0x8B30 };
        unsigned id{0};

        ShaderStage(Stage stage, const path&);
        ShaderStage(Stage stage, std::string_view src);

        ShaderStage(ShaderStage&&) = default;
        ~ShaderStage();
    };
 
    class Shader {
        friend Render;
        public:
            enum Type {
                Int, Float, Vec2, Vec3, Vec4, Mat3, Mat4, Texture
            };

            struct UniformConfig { std::string_view name; Type type; const void* data; };

            Shader() noexcept = default;
            Shader(list<ShaderStage> stages, list<UniformConfig> uniforms) noexcept;
        private:
            struct Uniform {
                unsigned short layout;
                Type type;
                const void* data;
            };

            unsigned program{0};
            Uniform* uniData{nullptr};
            unsigned uniCount{0};
        
            void Apply() const noexcept;
    };

    class Texture {
        friend Shader;
        friend Render;
        friend Framebuffer;
        unsigned id{0}; 
        int w{0}, h{0};

        public:
            Texture() = default;
            Texture(const path& path);

            static Texture Color(int width, int height) noexcept;
            static Texture Depth(int x, int y, bool compare = true) noexcept;
    };

    class Framebuffer {
        friend Render;
        unsigned fbo{0};
        int w{0}, h{0};
        bool color_attached{false};

        public:
            Framebuffer() = default;
            Framebuffer(int width, int height) noexcept;

                void AttachColor(const Texture&) noexcept;
                void AttachDepth(const Texture&) noexcept;
    };

            class Render {
        public:
            enum DrawTarget { None = 0, Front = 0x404, Back = 0x405 };

            void Target(const Framebuffer&, DrawTarget target = None) const noexcept;
            void Fill(vec3 colour) const noexcept;
            void Draw(const Shader& shader, const pk::Mesh& geometry, pk::span<Model> models) const noexcept;
            void Draw(const Shader& shader) const noexcept;
            void Swap() const noexcept;
            void Clear(int flags) const noexcept;

            void Viewport() const noexcept;
            void Viewport(int w, int h) const noexcept;
            void Viewport(int x, int y, int w, int h) const noexcept;
            Render() = default;
            Render(Render&&) = default;
            Render& operator=(Render&&) = default;
    };

    inline Framebuffer screen;
}