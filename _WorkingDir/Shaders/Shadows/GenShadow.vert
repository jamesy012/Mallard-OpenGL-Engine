#version 410

layout(location=0) in vec4 position;

uniform mat4 projectionViewMatrix;
uniform mat4 model = mat4(1);

void main() {
    gl_Position = projectionViewMatrix * model * position;
}
