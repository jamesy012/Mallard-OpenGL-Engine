#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;
layout(location=2) in vec2 texCoords;
layout(location=3) in vec4 vertexColor;

out vec4 vNormal;
out vec4 vShadowCoord;
out vec2 vTexCoord;
out vec4 vVertexColor;
out vec4 vWorldPosition;

uniform mat4 projectionViewMatrix;
uniform mat4 models[128];
uniform mat4 normalRot = mat4(1);
uniform vec4 offset;

uniform mat4 lightMatrix;

void main() {

    gl_Position = projectionViewMatrix * models[gl_InstanceID] * position;
	gl_Position += projectionViewMatrix * offset;
	
	vShadowCoord = lightMatrix * models[gl_InstanceID] * position;
	vShadowCoord += lightMatrix * offset;
	
		
    vNormal = normalRot * normal;    
    vTexCoord = texCoords;
	vVertexColor = vertexColor;
	vWorldPosition = models[gl_InstanceID] * position;
}
