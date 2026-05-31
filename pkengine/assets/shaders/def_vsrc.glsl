out vec2 uv;
void main() {
    mat4 MVP = P * V * model();
    gl_Position = MVP * vec4(_pos, 1.0);
    uv = _uv;
}