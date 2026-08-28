#pragma once
#include <PK/model.hpp>
#include <PK/mesh.hpp>
#include <string_view>
#include <filesystem>
#include <initializer_list>

namespace pk {
    using path = std::filesystem::path;

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

            void draw(const pk::Mesh& geometry, pk::span<Model> models) const noexcept;
    };

    class Texture {
        friend ShaderProgram;
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif
            
        unsigned id{0};
        public:
            //~Texture();
            Texture() = default;
            Texture(const path& path);
            Texture(Texture&& b) = default; 
            Texture& operator=(Texture&&) = default;
    };
}