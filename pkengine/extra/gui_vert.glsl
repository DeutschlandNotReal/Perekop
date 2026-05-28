#version 330
in vec2 vpos;
in float Z;
in vec2 pos;
in vec2 size;
in vec3 col;
out vec3 col2;

void main() {
    col2 = col;
    gl_Position = vec4(2.0 * (pos + size * vpos) - vec2(1), Z, 1.0);
}