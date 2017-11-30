#version 410

layout(location=0) in vec4 position;
layout(location=3) in mat4 model;

uniform mat4 projectionViewMatrix;
//uniform mat4 model = mat4(1);

void main() {
    gl_Position = projectionViewMatrix * model * position;
}
