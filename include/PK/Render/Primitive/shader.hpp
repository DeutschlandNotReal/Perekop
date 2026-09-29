#pragma once
#include <PK/Physics/pose.hpp>
#include <string_view>
#include <filesystem>
#include <initializer_list>

namespace pk::Render {
    enum class ShaderType {
        Vertex   = 0x8B31,
        Fragment = 0x8B30
    };

    class ShaderStage {
        friend struct Renderer;
        friend class Shader;

        unsigned index{0};

        public:
            ShaderStage(ShaderType type, const std::filesystem::path&) noexcept;
            ShaderStage(ShaderType type, std::string_view) noexcept;

            unsigned GetIndex() const noexcept {
                return index;
            }
    };

    class Shader {
        friend struct Renderer;
        unsigned index{0};
        
        public:
            class Uniform {
                friend Shader;
                friend struct Renderer;

                unsigned program{0}, index{0};

                Uniform(unsigned Program, std::string_view name) noexcept;
            
                public:
                    int    operator=(int)    noexcept;
                    float  operator=(float)  noexcept;
                    vec2   operator=(vec2)   noexcept;
                    vec3   operator=(vec3)   noexcept;
                    vec4   operator=(vec4)   noexcept;
                    mat3   operator=(mat3)   noexcept;
                    mat4   operator=(mat4)   noexcept;

                    unsigned GetIndex() const noexcept {
                        return index;
                    }
            };

            Shader() noexcept = default;
            Shader(std::initializer_list<ShaderStage> stages) noexcept;

            void Use() const noexcept;
            Uniform GetUniform(std::string_view name) const noexcept;
            void SetTexture(std::string_view name, unsigned slot, const class Texture&) noexcept;
    };
}