#version 410

in vec2 vTexCoord;

out vec4 fragColor;

uniform sampler2D TexDiffuse1;

void main() {
    fragColor = texture(TexDiffuse1, vTexCoord);
}
