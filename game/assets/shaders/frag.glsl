#version 430
out vec4 fragColor;

uniform sampler2D T;

in vec2 f_uv;

void main() { 
    fragColor = texture(T, f_uv);
}