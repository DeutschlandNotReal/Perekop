uniform float t;
uniform vec3 bgcol;
out vec3 col;

void main() { 
    vec3 sunvec = vec3(0,cos(t),sin(t));

    vec4 _view = V * model() * vec4(_pos, 1.0);
    gl_Position = P * _view;
    float a = dot(_norm, sunvec) * 0.5 + 0.5;
    col = mix(vec3(
        meta.x * _uv.x * a, 
        meta.y * _uv.y * a, 
        meta.z * (_uv.x + _uv.y) * 0.5 * a
    ), bgcol, clamp(1.0-exp(_view.z * 0.1), 0, 1)) * texture(_texture, _uv).xyz;
}