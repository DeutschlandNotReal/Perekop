in vec2 uv;
uniform sampler2D _texture;

void main() { 
    fragColor = texture(_texture, uv); 
}