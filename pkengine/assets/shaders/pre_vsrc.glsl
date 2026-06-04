#version 430
// per vertex:
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_nor;
layout(location = 2) in vec2 v_uv;

// per instance:
layout(location = 3) in mat4 model;
layout(location = 7) in vec4 metadata;

// per draw:
uniform mat4 view;
uniform mat4 proj;