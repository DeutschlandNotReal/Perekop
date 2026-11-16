#version 430
out vec4 fragColor;

uniform sampler2D T;

in vec2 f_uv;
in vec3 f_normal;
in vec3 f_tint;

void main() {
    fragColor = texture(T, f_uv);
}