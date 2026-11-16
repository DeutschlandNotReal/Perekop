#version 430
layout(location = 0) in mat3 i_rot;
layout(location = 3) in vec3 i_ext;
layout(location = 4) in vec3 i_pos;
layout(location = 5) in vec4 i_col;

uniform mat4 camera;
out vec4 colour;

const int tris[36] = int[](
    0,4,2, 2,4,6,
    1,3,5, 3,7,5,
    0,1,4, 1,5,4,
    2,6,3, 3,6,7,
    0,2,1, 1,2,3,
    4,5,6, 5,7,6
);

void main() {
    int i = tris[gl_VertexID];
    vec3 corner = vec3(i & 1, (i >> 1) & 1, (i >> 2) & 1) * 2.0 - 1.0;

    gl_Position = camera * vec4(i_rot * (corner * i_ext) + i_pos, 1.0);
    colour = i_col;
}