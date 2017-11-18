#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;

out vec4 vNormal;
out vec4 vShadowCoord;

uniform mat4 projectionViewMatrix;
uniform mat4 model = mat4(1);
uniform mat4 normalRot = mat4(1);

uniform mat4 lightMatrix;

void main() {
    gl_Position = projectionViewMatrix * model * position;
    vNormal = normalRot * normal;
    vShadowCoord = lightMatrix * model * position
}
