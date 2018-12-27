#version 410

layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 3 ) out;

out vec3 geomColour;
in vec4 vPos[];

uniform mat4 projectionViewMatrix;
uniform mat4 model = mat4(1);

uniform vec3 lightDirection = normalize(vec3(0.4, -1.0, 0.8));
uniform float ambientLighting = 0.3;

vec3 calculateTriangleNormal(){
	vec3 tangent = vPos[1].xyz - vPos[0].xyz;
	vec3 bitangent = vPos[2].xyz - vPos[0].xyz;
	return normalize(cross(tangent, bitangent));	
}

vec3 calculateLighting(vec3 faceNormal){
	float brightness = max(dot(-lightDirection, faceNormal), ambientLighting);
	return vec3(brightness,brightness,brightness);
}

void main(){

vec3 normal = calculateTriangleNormal();
geomColour = calculateLighting(normal);

gl_Position = gl_in[0].gl_Position;
EmitVertex();
gl_Position = gl_in[1].gl_Position;
EmitVertex();
gl_Position = gl_in[2].gl_Position;
EmitVertex();
EndPrimitive();

}