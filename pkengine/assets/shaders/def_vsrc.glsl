out vec3 col;
void main() {
    mat4 MVP = P * V * model();
    gl_Position = MVP * vec4(_pos, 1.0);
    col = texture(_texture, _uv).xyz;
}