#version 410

layout(location=0) in vec4 position;

uniform mat4 projectionViewMatrix;
uniform mat4 models[128];
uniform vec4 offset;

void main() {
    gl_Position = projectionViewMatrix * models[gl_InstanceID] * position;
	gl_Position += projectionViewMatrix * offset;
}
