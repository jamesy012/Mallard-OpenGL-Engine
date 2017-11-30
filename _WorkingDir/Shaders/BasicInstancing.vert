#version 410
layout(location = 0) in vec4 position;	
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec4 vertColor;	

uniform mat4 projectionViewMatrix;
uniform mat4 models[128];
uniform vec4 offset;

out vec2 vTexCoord;
out vec4 vVertColor;

void main() {
	gl_Position = projectionViewMatrix * models[gl_InstanceID] * position;
    gl_Position += projectionViewMatrix * offset;
	vTexCoord = texCoord; 
	vVertColor = vertColor;
}
