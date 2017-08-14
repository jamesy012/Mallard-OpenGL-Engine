#version 410

layout(location = 2) in vec2 texCoord;

out vec2 vTexCoord;

void main() {
    vec2 coords = (texCoord * 2.0) - 1.0;
	gl_Position = vec4(coords, 0.0, 1.0);
	vTexCoord = texCoord;
}