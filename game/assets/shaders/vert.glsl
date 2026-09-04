#version 430
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_nor;
layout(location = 2) in vec2 v_uv;

layout(location = 3) in mat4 model;
layout(location = 7) in vec3 scale;
layout(location = 8) in vec4 metadata;

uniform mat4 camera;
uniform mat4 light;
uniform vec3 lightPos;

out vec2 f_uv;
out vec4 f_light;
out vec3 f_normal;
out vec3 f_world;
out vec3 f_toLight;

void main() {
    vec4 world = model * vec4(v_pos * scale, 1.0);
    gl_Position = camera * world;
    f_light = light * world;
    f_uv = v_uv;
    f_world = world.xyz;
    f_normal = normalize(mat3(model) * v_nor);
    f_toLight = lightPos - world.xyz;
}