#pragma once
#include <PK/Math/pose.hpp>
#include <filesystem>
#include <initializer_list>
#include <PK/Render/Resources/texture.hpp>
#include <source_location>

namespace pk {
    enum class ShaderType {
        vertex   = 0x8B31,
        fragment = 0x8B30
    };

    class ShaderStage {
        friend struct Renderer;
        friend class shader;

        uint index{0};

        public:
            ShaderStage(ShaderType type, const std::filesystem::path&) noexcept;
            ShaderStage(ShaderType type, std::string_view) noexcept;

            unsigned getIndex() const noexcept {
                return index;
            }
    };

    class Shader {
        friend struct Renderer;
        uint index{0};
        
        public:
            class uniform {
                friend Shader;
                friend struct Renderer;

                uint program{0}, index{0};

                uniform(uint program, std::string_view name, std::source_location loc = std::source_location::current()) noexcept;
            
                public:
                    int   operator=(int)   noexcept;
                    float operator=(float) noexcept;
                    vec2  operator=(vec2)  noexcept;
                    vec3  operator=(vec3)  noexcept;
                    vec4  operator=(vec4)  noexcept;
                    mat3  operator=(mat3)  noexcept;
                    mat4  operator=(mat4)  noexcept;

                    uint getIndex() const noexcept {
                        return index;
                    }
            };

            Shader() noexcept = default;
            Shader(std::initializer_list<ShaderStage> stages) noexcept;

            void use() const noexcept;
            uniform getUniform(std::string_view name, std::source_location loc = std::source_location::current()) const noexcept;
            void setTexture(std::string_view name, uint slot, glRefference<Texture>) noexcept;
    };
}