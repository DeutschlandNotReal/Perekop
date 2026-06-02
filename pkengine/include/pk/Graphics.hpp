#pragma once
#include <pkutil/Array.hpp>
#include <glm/matrix.hpp>

namespace pk {
    enum class Uniform {
        u_int, u_float, u_vec2, u_vec3, u_vec4, u_mat3, u_mat4
    };

    class Shader {
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif

        struct LUniform { int layout; Uniform type; const void* data; };
        uint program{0}, layoutP{0}, layoutV{0}, layoutT{0};
        Array<LUniform> uniforms;
        void use(const glm::mat4& V, const glm::mat4& P) const;
        public:
            Shader() = default;
            Shader(const char* vspath, const char* fspath);

            void uniform(Uniform type, const char* name, const void* data);
    };

    class Texture {
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif
        uint id{0};
        void use(uint layout) const;
        public:
            Texture() = default;
            Texture(const char* tpath);
    };
}