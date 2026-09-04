#version 430
out vec4 fragColor;

uniform sampler2D T;
uniform sampler2DShadow shadow;
uniform vec3 lightPos;

in vec2 f_uv;
in vec4 f_light;
in vec3 f_normal;
in vec3 f_world;
in vec3 f_toLight;

void main() {
    vec3 base = texture(T, f_uv).rgb;
    vec3 n = normalize(f_normal);
    vec3 light_dir = normalize(f_toLight);
    float diffuse = max(dot(n, light_dir), 0.0);

    vec3 shadow_pos = f_light.xyz / f_light.w;
    shadow_pos = shadow_pos * 0.5 + 0.5;

    float shadow_amount = 1.0;
    if (shadow_pos.x >= 0.0 && shadow_pos.x <= 1.0 &&
        shadow_pos.y >= 0.0 && shadow_pos.y <= 1.0 &&
        shadow_pos.z >= 0.0 && shadow_pos.z <= 1.0) {
        float bias = max(0.0035 * (1.0 - dot(n, light_dir)), 0.0005);
        shadow_amount = texture(shadow, vec3(shadow_pos.xy, shadow_pos.z - bias));
    }

    float ambient = 0.12;
    float pulse = 0.9 + 0.2 * sin((f_world.x + f_world.y + f_world.z) * 0.7 + 2.0);
    vec3 lit = base * (ambient + diffuse * shadow_amount * 1.8) * pulse;
    vec3 shadowed = base * 0.18;
    vec3 color = mix(shadowed, lit, clamp(diffuse * shadow_amount + 0.2, 0.0, 1.0));
    color += vec3(0.08, 0.08, 0.1);

    fragColor = vec4(color, 1.0);
}