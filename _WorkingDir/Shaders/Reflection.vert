#version 410
layout(location = 0) in vec4 position;
layout(location = 2) in vec2 texCoord;

uniform mat4 projectionViewMatrix;
uniform mat4 model = mat4(1);

out vec3 vPosition;
out vec3 vPosition2;

void main() {
	gl_Position = projectionViewMatrix * model * position;	
    
	vPosition =  (model * position).xzy;  
	vPosition2 =  (position).xzy;  
}