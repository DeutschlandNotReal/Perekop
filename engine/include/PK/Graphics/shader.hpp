#pragma once
#include <PK/Math/number.hpp>
#include <PK/Core/vector.hpp>
#include <PK/Core/string.hpp>
#include <PK/Math/mat.hpp>

namespace pk {
    enum class UniformType {
        u_int, u_float, u_vec2, u_vec3, u_vec4, u_mat3, u_mat4
    };

    class Shader {
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif

        struct Uniform { u16 layout; UniformType type; const void* data; };
        u32 program{0}, layoutP{0}, layoutV{0}, layoutT{0};

        vector<Uniform> uniforms;
        
        void use(const mat4& V, const mat4& P) const noexcept;
        
        public:
            Shader() = default;
            Shader(strview title, strview vshader_path, strview fshader_path);

            void uniform(UniformType type, strview title, const void* data);
    };
}