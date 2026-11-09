#version 430
out vec4 fragColor;

uniform sampler2D T;

in vec2 f_uv;
in vec3 f_normal;
in vec3 f_toLight;
in vec3 f_tint;

void main() {
    vec3 lightDirection = normalize(f_toLight);
    float diffuse = max(dot(normalize(f_normal), lightDirection), 0.0);
    vec3 colour = texture(T, f_uv).rgb * f_tint * (0.2 + 0.8 * diffuse);
    fragColor = vec4(colour, 1.0);
}