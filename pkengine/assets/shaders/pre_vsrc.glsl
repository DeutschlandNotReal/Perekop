#version 430
layout(location = 0) in vec3 _pos;
layout(location = 1) in vec3 _norm;
layout(location = 2) in vec2 _uv;
layout(location = 3) in vec4 M0;
layout(location = 4) in vec4 M1;
layout(location = 5) in vec4 M2;
layout(location = 6) in vec4 meta;
uniform mat4 V;
uniform mat4 P;

mat4 model() {
    return mat4(
        vec4(M0.xyz, 0.0), 
        vec4(M1.xyz, 0.0), 
        vec4(M2.xyz, 0.0), 
        vec4(M0.w, M1.w, M2.w, 1.0)
    );
};
