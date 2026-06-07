in vec4 f_colour;
in vec2 f_uv;
in float f_t;

uniform vec3 f_bgcol;
uniform sampler2D f_image;


void main() { 
    fragColor = mix(f_colour * texture(f_image, f_uv), vec4(f_bgcol, 1.0), f_t); 
    //fragColor = vec4(depth, depth, depth, 1.0);
}