#version 410

#include "defaultLayout_Instance.inc"

uniform vec4 offset;

out vec2 vTexCoord;
out vec4 vVertColor;

void main() {
	gl_Position = projectionViewMatrix * models[gl_InstanceID] * position;
    gl_Position += projectionViewMatrix * offset;
	vTexCoord = texCoord; 
	vVertColor = vertexColor;
}
