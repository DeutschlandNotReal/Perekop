#version 430
out vec4 fragColor;
in vec2 fuv;

uniform sampler2D T;

void main() { 
    fragColor = texture(T, fuv);
    //fragColor = vec4(1.0, 1.0, 0.0, 1.0);
}