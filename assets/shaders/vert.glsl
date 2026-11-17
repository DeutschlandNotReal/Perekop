#version 430
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_nor;
layout(location = 2) in vec2 v_uv;

uniform mat4 MVP;
uniform vec3 scale;
out vec2 f_uv;

void main() {
    gl_Position = MVP * vec4(v_pos * scale, 1.0);
    f_uv = v_uv;
}