#pragma once
#include <glm/matrix.hpp>

#include <PKCore/vector.hpp>
#include <PKCore/string.hpp>
#include <PKLib/Math.hpp>

namespace pk {
    enum class Uniform {
        u_int, u_float, u_vec2, u_vec3, u_vec4, u_mat3, u_mat4
    };

    class Shader {
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif

        struct LUniform { int layout; Uniform type; const void* data; };
        uint32_t program{0}, layoutP{0}, layoutV{0}, layoutT{0};
        vector<LUniform> uniforms;
        void use(const mat4& V, const mat4& P) const;
        
        public:
            Shader() = default;
            Shader(rstring title, rstring vspath, rstring fspath);

            void uniform(Uniform type, rstring title, const void* data);
    };

    class Texture {
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif
        uint32_t id{0};
        void use(uint32_t layout) const;
        public:
            Texture() = default;
            Texture(rstring path);
    };
}