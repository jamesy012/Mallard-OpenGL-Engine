#version 410

layout(location = 0) in vec4 position;
//layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec4 vertColor;	

uniform mat4 projectionViewMatrix;
uniform mat4 model = mat4(1);

//out vec2 vTexCoord;
out vec4 vVertColor;
out vec4 vPos;

void main() {	
	vPos = model * position;		
	gl_Position = projectionViewMatrix * vPos;
	//vTexCoord = texCoord;
	vVertColor = vertColor;
}
