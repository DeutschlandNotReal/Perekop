#version 430
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_nor;
layout(location = 2) in vec2 v_uv;

layout(location = 3) in mat4 model;
layout(location = 7) in vec4 metadata;

uniform mat4 view;
uniform mat4 proj;
uniform float time;

out vec4 f_colour;
out vec2 f_uv;
out float f_t;

void main() {
    vec4 viewspace = view * model * vec4(v_pos, 1.0);
    gl_Position = proj * viewspace;
    float depth = -viewspace.z;

    f_colour = vec4(metadata.xyz, 1.0);
    f_uv = v_uv;
    f_t = clamp(1.0 - exp(-depth * 0.05), 0, 1);
}