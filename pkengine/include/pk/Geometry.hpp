#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <pk/Pose.hpp>
#include <pkutil/SparseSet.hpp>
#include <pk/Graphics.hpp>

namespace pk {
    class Mesh {
        #ifdef PK_INTERNAL
        friend void Perekop::render(bool);
        #endif
        uint VBO{0}, EBO{0}, IBO{0};
        public:
            struct alignas(32) Vertex {  
                glm::vec3 pos;
                glm::vec3 normal;
                glm::vec2 uv;
            };
            short id{0};

            Texture texture;
            Shader* shader{nullptr};
            Array<Vertex> vertex;
            Array<short> indices;

            void load();
            void reload();
            void unload();
    };

    struct Model { 
        short id, mesh{0}, body{0};
        Pose pose;
        glm::vec4 metadata;
    };

    struct Camera { 
        float min{.1}, max{200}, fov{75}; 
        Pose pose;

        glm::mat4 view() const { 
            return glm::inverse((glm::mat4)pose); 
        }

        glm::mat4 proj(float aspect) const { 
            return glm::perspective(glm::radians(fov), aspect, min, max); 
        }
    };
}