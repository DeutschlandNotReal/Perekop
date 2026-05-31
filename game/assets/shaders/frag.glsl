in vec3 col;
in vec2 uv;
uniform sampler2D _texture;

void main() { 
    fragColor = vec4(col, 1.0) * texture(_texture, uv); 
}