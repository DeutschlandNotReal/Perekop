#version 430
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_nor;
layout(location = 2) in vec2 v_uv;
layout(location = 3) in mat4 instancePose;
layout(location = 7) in vec4 instanceTint;

uniform mat4 model;
uniform vec3 scale;
uniform mat4 camera;
uniform vec3 lightPos;
uniform vec3 tint;
uniform vec3 arrowShape;
uniform int instanced;

out vec2 f_uv;
out vec3 f_normal;
out vec3 f_toLight;
out vec3 f_tint;

void main() {
    mat4 transform = instanced != 0 ? instancePose : model;
    vec3 objectScale = instanced != 0 ? vec3(1.0) : scale;
    vec3 position = v_pos * objectScale;
    if (instanced != 0) {
        float coneLength = arrowShape.z - arrowShape.y;
        float baseLength = max(instanceTint.a - coneLength, 0.0);
        float coneScale = coneLength > 0.0 ? min(instanceTint.a / coneLength, 1.0) : 1.0;
        position.y = position.y <= arrowShape.y
            ? (position.y - arrowShape.x) * baseLength / max(arrowShape.y - arrowShape.x, 1e-5)
            : baseLength + (position.y - arrowShape.y) * coneScale;
    }
    vec4 world = transform * vec4(position, 1.0);
    gl_Position = camera * world;
    f_uv = v_uv;
    f_normal = normalize(mat3(transform) * v_nor);
    f_toLight = lightPos - world.xyz;
    f_tint = instanced != 0 ? instanceTint.rgb : tint;
}