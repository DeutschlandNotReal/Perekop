#pragma once
#include <PK/model.hpp>
#include <PK/mesh.hpp>
#include <string_view>
#include <filesystem>
#include <initializer_list>

namespace pk {
    using path = std::filesystem::path;
    class Renderer;

    enum BufferType : unsigned {
        colourBuffer = 0x4000,
        depthBuffer  = 0x0100,
    };


    struct Shader {
        enum type { vertex = 0x8B31, fragment = 0x8B30 };
        unsigned id{0};

        Shader(type T, const path&);
        Shader(type T, std::string_view src);
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        ~Shader();
    };
 
    class ShaderProgram {
        friend Renderer;
        public:
            enum UDataType {
                u_int, u_float, u_vec2, u_vec3, u_vec4, u_mat3, u_mat4, texture
            };

            struct UniformConfig { std::string_view name; UDataType type; const void* data; };
        private:
            #ifdef PK_INTERNAL
            friend void Perekop::render(bool);
            #endif

            struct Uniform { unsigned short layout; UDataType type; const void* data; };
            unsigned program{0};
            Uniform* uniform; unsigned uniform_n;
        
            void apply() const noexcept;
        
        public:
            ShaderProgram() noexcept = default;

            ShaderProgram(const Shader& v, const Shader& f, std::initializer_list<UniformConfig> u) noexcept;
    };

    class Texture {
        friend ShaderProgram;
        friend Renderer;
            
        unsigned id{0};
        public:
            Texture() = default;
            Texture(const path& path);
    };

    class Target {
        friend Renderer;
        unsigned fbo{0}, depth{0}, colour{0};
        int x{0}, y{0};

        public:
            Target() = default;
            Target(int width, int height, unsigned flags) noexcept;
    };

    class Renderer {
        struct ShaderModel { mat4 mat; vec3 scl; vec4 meta; };
        pk::vector<ShaderModel> modelcache;

        public:
            void target(const Target&) const noexcept;
            void fill(vec3 colour) const noexcept;
            void ready_models(pk::span<Model> models) noexcept;
            void ready_models(pk::span<Model> models, mat4 t) noexcept;
            void draw(const ShaderProgram& shader, const pk::Mesh& geometry) const noexcept;
            void swap() const noexcept;
            void clear(int flags) const noexcept;
            Renderer() = default;
            Renderer(Renderer&&) = default;
            Renderer& operator=(Renderer&&) = default;
    };

    inline Target screen;
}