#version 430
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_nor;
layout(location = 2) in vec2 v_uv;

layout(location = 3) in mat4 model;
layout(location = 7) in vec3 scale;
layout(location = 8) in vec4 metadata;

uniform mat4 camera;

out vec2 f_uv;

void main() {
    gl_Position = camera * model * vec4(v_pos * scale, 1.0);
 
    f_uv = v_uv;
}