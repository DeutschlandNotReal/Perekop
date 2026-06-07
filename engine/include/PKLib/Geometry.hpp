#pragma once
#include <PKLib/Math.hpp>
#include <PKLib/Pose.hpp>
#include <PKLib/Graphics.hpp>

namespace pk {
    class Mesh {
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif
        uint32_t VBO{0}, EBO{0}, IBO{0};

        mat3 inertia{0};
        vec3 lbound, hbound;

        public:
            struct alignas(32) Vertex { vec3 p, n; vec2 uv; };
            short id{0};

            Texture texture;
            Shader* shader{nullptr};
            
            array<Vertex> vertices;
            array<short> indices;

            void load();
            void unload();
    };

    struct Model { 
        short id, mesh{0}, body{0};
        Pose pose;
        vec4 metadata;
    };

    struct Camera { 
        float min{.01}, max{200}, fov{glm::radians(70.f)}; 
        Pose pose;

        mat4 view() const { 
            return glm::inverse(pose.mat4()); 
        }

        mat4 proj(float aspect) const { 
            return glm::perspective(fov, aspect, min, max); 
        }
    };
}