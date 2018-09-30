#version 410

layout ( triangles ) in;
layout ( line_strip, max_vertices = 6 ) out;

const float MAGNITUDE = 1;

uniform mat4 model = mat4(1);

in vec3 vPos[];

in VS_OUT {
    vec3 normal;
} gs_in[];

vec3 normalTest;

vec3 calculateTriangleNormal(){
	vec3 tangent = vPos[1] - vPos[0];
	vec3 bitangent = vPos[2] - vPos[0];
	return normalize(cross(tangent, bitangent));	
}

void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    //gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE;
    gl_Position = gl_in[index].gl_Position + vec4(normalTest, 0.0) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}

void main()
{
	normalTest = calculateTriangleNormal();
    // GenerateLine(0); // first vertex normal
    // GenerateLine(1); // second vertex normal
    // GenerateLine(2); // third vertex normal

	vec4 pos = gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position;
	pos /= 3; 
	gl_Position = pos;
    EmitVertex();
    //gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE;
    gl_Position = pos + vec4(normalTest, 0.0) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}  