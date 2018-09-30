#version 410

layout(location = 0) in vec4 position;
//layout(location = 2) in vec2 texCoord;
layout(location = 1) in vec4 normal;	

uniform mat4 projectionViewMatrix;
uniform mat4 model = mat4(1);

out VS_OUT {
    vec3 normal;
} vs_out;
out vec3 vPos;
void main() {												
	gl_Position = projectionViewMatrix * model * position;
	//mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    //vs_out.normal = normalize(vec3(projection * vec4(normalMatrix * normal, 0.0)));
    //vs_out.normal = normalize(vec3(projectionViewMatrix * normal));
    //vs_out.normal = vec3(0,1,0);
    vs_out.normal = normal.xyz;
	vPos = vec3(position);
}
