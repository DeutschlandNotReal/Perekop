#version 430
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_nor;
layout(location = 2) in vec2 v_uv;

layout(location = 3) in mat4 model;
layout(location = 7) in vec3 scale;
layout(location = 8) in vec4 metadata;

uniform mat4 V;
uniform mat4 P;
out vec2 fuv;

void main() {
    vec4 viewspace = V * model * vec4(v_pos * scale, 1.0);
    gl_Position = P * viewspace;
 
    fuv = v_uv;
}