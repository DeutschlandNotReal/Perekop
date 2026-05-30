#version 430
layout(location = 0) in vec2 vpos;
layout(location = 1) in float Z;
layout(location = 2) in vec2 pos;
layout(location = 3) in vec2 size;
layout(location = 4) in vec4 col;
out vec4 col2;

void main() {
    col2 = col;
    gl_Position = vec4(2.0 * (pos + size * vpos) - vec2(1), Z, 1.0);
}