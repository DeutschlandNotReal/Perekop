uniform float t;
out vec4 col;

void main() { 
    gl_Position = VP * model() * vec4(_pos, 1.0); 
    col = vec4(_uv.x, _uv.y, fract(t), 1.0); 
};