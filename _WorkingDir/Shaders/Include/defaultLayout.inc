layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;
layout(location=2) in vec2 texCoord;
layout(location=3) in vec4 vertexColor;

uniform mat4 projectionViewMatrix = mat4(1);
uniform mat4 model = mat4(1);