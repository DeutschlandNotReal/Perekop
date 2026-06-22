#pragma once
#include <PKCore/vector.hpp>
#include <PKCore/string.hpp>
#include <PKLib/math.hpp>

namespace pk {
    enum class UniformType {
        u_int, u_float, u_vec2, u_vec3, u_vec4, u_mat3, u_mat4
    };

    class Shader {
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif

        struct Uniform { int layout; UniformType type; const void* data; };
        uint32_t program{0}, layoutP{0}, layoutV{0}, layoutT{0};
        vector<Uniform> uniforms;
        void use(const mat4& V, const mat4& P) const;
        
        public:
            Shader() = default;
            Shader(strview title, strview vspath, strview fspath);

            void uniform(UniformType type, strview title, const void* data);
    };
}