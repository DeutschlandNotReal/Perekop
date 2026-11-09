#pragma once
#include <PK/Math/pose.hpp>
#include <filesystem>
#include <initializer_list>

namespace pk {
    enum class shaderType {
        vertex   = 0x8B31,
        fragment = 0x8B30
    };

    class shaderStage {
        friend struct renderer;
        friend class shader;

        uint index{0};

        public:
            shaderStage(shaderType type, const std::filesystem::path&) noexcept;
            shaderStage(shaderType type, std::string_view) noexcept;

            unsigned getIndex() const noexcept {
                return index;
            }
    };

    class shader {
        friend struct renderer;
        uint index{0};
        
        public:
            class uniform {
                friend shader;
                friend struct renderer;

                uint program{0}, index{0};

                uniform(uint program, std::string_view name) noexcept;
            
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

            shader() noexcept = default;
            shader(std::initializer_list<shaderStage> stages) noexcept;

            void use() const noexcept;
            uniform getUniform(std::string_view name) const noexcept;
            void setTexture(std::string_view name, uint slot, const class texture&) noexcept;
    };
}