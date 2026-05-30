uniform float t;
out vec4 col;

void main() { 
    mat4 MVP = P * V * model();
    vec3 sunvec = normalize(vec3(
        0,
        cos(t),
        sin(t)
    ));

    gl_Position = MVP * vec4(_pos, 1.0); 
    float a = dot(_norm, sunvec) * 0.5 + 0.5;
    col = vec4(
        meta.x * _uv.x * a, 
        meta.y * _uv.y * a, 
        meta.z * (_uv.x + _uv.y) * 0.5 * a,
        1.0
    );
};