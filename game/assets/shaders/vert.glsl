uniform float t;
out vec3 col;
out float a;
out vec2 uv;

void main() { 
    vec4 VM = V * model() * vec4(_pos, 1.0);
    gl_Position = P * VM;
    float t = clamp(1.0-exp(VM.z * 0.1), 0, 1);
    col = mix(meta.xyz, vec3(0, 0, 0), t);
    uv = _uv;
}