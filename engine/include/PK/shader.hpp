#pragma once
#include <PKSTL/vector.hpp>
#include <PK/pose.hpp>
#include <string_view>
#include <filesystem>

namespace pk {
    using path = std::filesystem::path;

    enum class UniformType {
        u_int, u_float, u_vec2, u_vec3, u_vec4, u_mat3, u_mat4
    };

    class Shader {
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif

        struct Uniform { unsigned short layout; UniformType type; const void* data; };
        unsigned program{0}, layoutP{0}, layoutV{0}, layoutT{0};

        vector<Uniform> uniforms;
        
        void use(const mat4& V, const mat4& P) const noexcept;
        
        public:
            Shader() = default;
            Shader(std::string_view title, path vshader_path, path fshader_path);

            void uniform(UniformType type, std::string_view title, const void* data);
    };
}