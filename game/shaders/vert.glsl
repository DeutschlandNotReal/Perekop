uniform float t;
out vec4 col;

void main() { 
    mat4 MVP = P * V * model();
    gl_Position = MVP * vec4(_pos, 1.0); 
    vec3 n = normalize((MVP * vec4(_norm, 0.0)).xyz);
    float f = fract(dot(n, vec3(0.5, 0.5, 0.1)));
    col = vec4(_uv.x * f, _uv.y * f, 0.5 + 0.5 * sin(t), 1.0); 
};